#include "solver.hpp"

#include "ast/generator.hpp"
#include "format.hpp"
#include "pretty_printer.hpp"
#include "utility.hpp"

#include <cassert>

#ifdef _MSC_VER
#define __builtin_expect(cond, value) (cond)
#endif

namespace LTL {

namespace detail {

namespace colors = format::colors;

Solver::Solver(FormulaPtr formula, FrameID maximum_depth)
  : _formula(formula),
    _maximum_depth(maximum_depth),
    _state(State::UNINITIALIZED),
    _result(Result::UNDEFINED),
    _start_index(0),
    _loop_state(0),
    _stats(),
    _has_eventually(true),
    _has_until(true),
    _has_release(true)
{
  _initialize();
}

// Forward declaration
static bool formula_ordering_func(const FormulaPtr& a, const FormulaPtr& b);

// TODO: Break this down
void Solver::_initialize()
{
  format::debug("Initializing solver...");
  _atom_set.clear();

  /* Simplify the formula and put it in normal form */
  format::debug("Simplifing formula...");
  Simplifier simplifier;
  _formula = simplifier.simplify(_formula);

  /* Generate every subformulas */
  format::debug("Generating subformulas...");
  Generator gen;
  gen.generate(_formula);
  _subformulas = gen.formulas();

  /* The simplification might just have produces a True or False */
  if (_subformulas.size() == 1) {
    if (isa<True>(_subformulas[0])) {
      std::cout << "^0:" << clock() << std::endl;
      std::cerr << "Issat!"  << std::endl; 
      _result = Result::SATISFIABLE;
      _state = State::DONE;
      return;
    }
    else if (isa<False>(_subformulas[0])) {
      _result = Result::UNSATISFIABLE;
      std::cout << "^0:" << clock() << std::endl;
      std::cerr << "Unsat!"  << std::endl; 
      _state = State::DONE;
      return;
    }
  }

  /* Sort the subformulas in an order suitable for the computation and remove the duplicates */
  std::sort(_subformulas.begin(), _subformulas.end(), formula_ordering_func);

  auto last = std::unique(_subformulas.begin(), _subformulas.end());
  _subformulas.erase(last, _subformulas.end());

  format::debug("Found {} subformulas", _subformulas.size());
  format::debug("Building data structure...");

  /* Initialize the bitsets and arrays used to represent the subformulas */
  FormulaID current_index(0);

  _number_of_formulas = _subformulas.size();
  _bitset.atom.resize(_number_of_formulas);
  _bitset.negation.resize(_number_of_formulas);
  _bitset.tomorrow.resize(_number_of_formulas);
  _bitset.yesterday.resize(_number_of_formulas);
  _bitset.always.resize(_number_of_formulas);
  _bitset.eventually.resize(_number_of_formulas);
  _bitset.conjunction.resize(_number_of_formulas);
  _bitset.disjunction.resize(_number_of_formulas);
  _bitset.until.resize(_number_of_formulas);
  _bitset.release.resize(_number_of_formulas);
  _bitset.since.resize(_number_of_formulas);
  _bitset.triggered.resize(_number_of_formulas);
  _bitset.temporary.resize(_number_of_formulas);

  _lhs = std::vector<FormulaID>(_number_of_formulas, FormulaID::max());
  _rhs = std::vector<FormulaID>(_number_of_formulas, FormulaID::max());

  for (const auto &f : _subformulas) {
    if (f == _formula)
      _start_index = current_index;

	// TODO: 0 may not be a good default value as an ID even though it's unused
    FormulaID lhs(0), rhs(0);
    FormulaPtr left = nullptr, right = nullptr;

    if (isa<Negation>(f)) {
      if (isa<Until>(fast_cast<Negation>(f)->formula())) {
        left = simplifier.simplify(make_negation(
          fast_cast<Until>(fast_cast<Negation>(f)->formula())->left()));
        right = simplifier.simplify(make_negation(
          fast_cast<Until>(fast_cast<Negation>(f)->formula())->right()));
      }
      else
        left = fast_cast<Negation>(f)->formula();
    }
    else if (isa<Tomorrow>(f))
      left = fast_cast<Tomorrow>(f)->formula();
    else if (isa<Always>(f))
      left = fast_cast<Always>(f)->formula();
    else if (isa<Eventually>(f))
      left = fast_cast<Eventually>(f)->formula();
    else if (isa<Conjunction>(f)) {
      left = fast_cast<Conjunction>(f)->left();
      right = fast_cast<Conjunction>(f)->right();
    }
    else if (isa<Disjunction>(f)) {
      left = fast_cast<Disjunction>(f)->left();
      right = fast_cast<Disjunction>(f)->right();
    }
    else if (isa<Until>(f)) {
      left = fast_cast<Until>(f)->left();
      right = fast_cast<Until>(f)->right();
    }
    else if (isa<Then>(f))
      assert(false);
    else if (isa<Iff>(f))
      assert(false);

    if (left)
      lhs = FormulaID(static_cast<uint64_t>(
        std::lower_bound(_subformulas.begin(), _subformulas.end(), left, formula_ordering_func) -
		_subformulas.begin()));
    if (right)
      rhs = FormulaID(static_cast<uint64_t>(
        std::lower_bound(_subformulas.begin(), _subformulas.end(), right, formula_ordering_func) -
        _subformulas.begin()));

    _add_formula_for_position(f, current_index++, lhs, rhs);
  }

  /* Generate every possible eventualities beforehand and the look-up tables */
  format::debug("Generating eventualities...");
  _fw_eventualities_lut =
    std::vector<FormulaID>(_number_of_formulas, FormulaID::max());
  std::vector<FormulaPtr> eventualities;
  for (uint64_t i = 0; i < _subformulas.size(); ++i) {
    if (_bitset.eventually[i])
      eventualities.push_back(_subformulas[_lhs[i]]);
    else if (_bitset.until[i])
      eventualities.push_back(_subformulas[_rhs[i]]);
    else if (_bitset.release[i]) {
      eventualities.push_back(_subformulas[_lhs[i]]);
      eventualities.push_back(_subformulas[_rhs[i]]);
    }
  }

  std::sort(eventualities.begin(), eventualities.end(), formula_ordering_func);
  last = std::unique(eventualities.begin(), eventualities.end());
  eventualities.erase(last, eventualities.end());

  _bw_eventualities_lut = std::vector<FormulaID>(eventualities.size());
  for (uint64_t i = 0; i < eventualities.size(); ++i) {
    uint64_t position = static_cast<uint64_t>(
      std::lower_bound(_subformulas.begin(), _subformulas.end(),
                       eventualities[i], formula_ordering_func) -
      _subformulas.begin());
    _fw_eventualities_lut[position] = FormulaID(i);
    _bw_eventualities_lut[i] = FormulaID(position);
  }

  format::debug("Found {} eventualities", eventualities.size());

  /* We are now ready to start the computation */
  _has_eventually = _bitset.eventually.any();
  _has_until = _bitset.until.any();
  _has_release = _bitset.release.any();

  _stack.push(Frame(FrameID(0), _start_index, _number_of_formulas,
                    _bw_eventualities_lut.size()));
  _state = State::INITIALIZED;

  format::debug("Solver initialized!");
}

// TODO: The logic in this can be simplified
void Solver::_add_formula_for_position(const FormulaPtr &formula, FormulaID position, FormulaID lhs, FormulaID rhs)
{
  switch (formula->type()) {
    case Formula::Type::Atom:
      _bitset.atom[position] = true;
      _atom_set[position] = fast_cast<Atom>(formula)->name();
      break;

    case Formula::Type::Negation:
      if (isa<Until>(fast_cast<Negation>(formula)->formula())) {
        _bitset.release[position] = true;
        _lhs[position] = lhs;
        _rhs[position] = rhs;
        break;
      }
      _bitset.negation[position] = true;
      _lhs[position] = lhs;
      break;

    case Formula::Type::Tomorrow:
      _bitset.tomorrow[position] = true;
      _lhs[position] = lhs;
      break;

    case Formula::Type::Yesterday:
      _bitset.yesterday[position] = true;
      _lhs[position] = lhs;
      break;

    case Formula::Type::Always:
      _bitset.always[position] = true;
      _lhs[position] = lhs;
      break;

    case Formula::Type::Eventually:
      _bitset.eventually[position] = true;
      _lhs[position] = lhs;
      break;

    case Formula::Type::Conjunction:
      _bitset.conjunction[position] = true;
      _lhs[position] = lhs;
      _rhs[position] = rhs;
      break;

    case Formula::Type::Disjunction:
      _bitset.disjunction[position] = true;
      _lhs[position] = lhs;
      _rhs[position] = rhs;
      break;

    case Formula::Type::Until:
      _bitset.until[position] = true;
      _lhs[position] = lhs;
      _rhs[position] = rhs;
      break;

    case Formula::Type::Release:
      _bitset.release[position] = true;
      _lhs[position] = lhs;
      _rhs[position] = rhs;
      break;

    case Formula::Type::Since:
      _bitset.since[position] = true;
      _lhs[position] = lhs;
      _rhs[position] = rhs;
      break;

    case Formula::Type::Triggered:
      _bitset.triggered[position] = true;
      _lhs[position] = lhs;
      _rhs[position] = rhs;
      break;

    case Formula::Type::Past:
      _bitset.past[position] = true;
      _lhs[position] = lhs;
      _rhs[position] = rhs;
      break;

    case Formula::Type::Historically:
      _bitset.historically[position] = true;
      _lhs[position] = lhs;
      _rhs[position] = rhs;
      break;

    case Formula::Type::True:
    case Formula::Type::False:
    case Formula::Type::Iff:
    case Formula::Type::Then:
      assert(false);
      break;
  }
}

bool Solver::_check_contradiction_rule()
{
  const Frame &frame = _stack.top();

  _bitset.temporary = frame.formulas;
  _bitset.temporary &= _bitset.negation;
  _bitset.temporary >>= 1;
  _bitset.temporary &= frame.formulas;
  return _bitset.temporary.any();
}

// TODO: extract every conjunction, even when nested
bool Solver::_apply_conjunction_rule()
{
  Frame &frame = _stack.top();
  _bitset.temporary = frame.formulas;
  _bitset.temporary &= _bitset.conjunction;
  _bitset.temporary &= frame.to_process;

  if (!_bitset.temporary.any())
    return false;

  // TODO: find_first and find_next don't use __builtin_clz/__builtin_ctz, find
  // if a custom implementation using them is faster
  size_t one = _bitset.temporary.find_first();
  while (one != Bitset::npos) {
    assert(_bitset.conjunction[one]);
    assert(frame.formulas[one]);
    assert(frame.to_process[one]);

    frame.formulas[_lhs[one]] = true;
    frame.formulas[_rhs[one]] = true;
    frame.to_process[one] = false;
    one = _bitset.temporary.find_next(one);
  }

  return true;
}

bool Solver::_apply_always_rule()
{
  Frame &frame = _stack.top();
  _bitset.temporary = frame.formulas;
  _bitset.temporary &= _bitset.always;
  _bitset.temporary &= frame.to_process;

  if (!_bitset.temporary.any())
    return false;

  size_t one = _bitset.temporary.find_first();
  while (one != Bitset::npos) {
    assert(_bitset.always[one]);
    assert(frame.formulas[one]);
    assert(frame.to_process[one]);

    frame.formulas[_lhs[one]] = true;
    assert(_bitset.tomorrow[one + 1] && _lhs[one + 1] == FormulaID(one));
    frame.formulas[one + 1] = true;
    frame.to_process[one] = false;
    one = _bitset.temporary.find_next(one);
  }

  return true;
}

#define APPLY_RULE(rule)                         \
  bool Solver::_apply_##rule##_rule()            \
  {                                              \
    Frame &frame = _stack.top();                 \
    _bitset.temporary = frame.formulas;          \
    _bitset.temporary &= _bitset.rule;           \
    _bitset.temporary &= frame.to_process;       \
                                                 \
    size_t one = _bitset.temporary.find_first(); \
    if (one != Bitset::npos) {                   \
      assert(_bitset.rule[one]);                 \
      assert(frame.formulas[one]);               \
      assert(frame.to_process[one]);             \
                                                 \
      frame.to_process[one] = false;             \
      frame.choosen_formula = FormulaID(one);     \
      frame.type = Frame::CHOICE;                \
      return true;                               \
    }                                            \
                                                 \
    return false;                                \
  }

APPLY_RULE(disjunction)
APPLY_RULE(eventually)
APPLY_RULE(until)
APPLY_RULE(release)

#undef APPLY_RULE

uint32_t width[1000]={0};

uint32_t num_jobs=1;

static uint32_t inline to_job (uint32_t i) {
        uint32_t m = (i % num_jobs);
        uint32_t r = (i / num_jobs);
        if (r) {
                srand(r); /*set random seed*/
                return((rand()+m)%num_jobs);
        }
        else {  
                return m;
        }
}

Solver::Result Solver::solution()
{
  if (_state == State::RUNNING || _state == State::DONE)
    return _result;

  if (_state == State::PAUSED)
    _rollback_to_latest_choice();

  _state = State::RUNNING;
  bool rules_applied;

uint32_t clock_depth=0;
if (getenv("CLOCK_DEPTH"))
     sscanf(getenv("CLOCK_DEPTH"),"%u",&clock_depth);

uint32_t job_no=1;
uint32_t split_depth=1;
uint32_t last_depth=0;
if (! getenv("JOB_NO") || sscanf(getenv("JOB_NO"),"%u/%u@%u",&job_no,&num_jobs,&split_depth)<3) {
    //std::cout << "\nTo use in parallel\n";
    std::cout << "USAGE: JOB_NO=[job_no]/[number_of_jobs]@[split_depth] checker ...\n";
    //std::cout << "e.g.: for i in 1 2 3; do JOB_NO=$i/3@7 checker ... ; done" << std::endl;
    //std::cout << "\nAn example of when this doen't work well (only 2x speed up)" << std::endl;
    //std::cout << "make && for i in `seq 1 1 7`; do JOB_NO=$i/17@7 timeout 30 time bin/checker -v 4  -l '(G (a => X (b|c))) & (G (b => X a)) & (G (c => X a)) & a & X X X X X X X X X X ~a'; done  2>&1 | tee out.txt" << std::endl;
    //exit(1);
}

loop:
  while (!_stack.empty()) {
    Frame &frame = _stack.top();
    //std::cout << "D" << _stack.size() << "," << frame.id << "," << not_reach_end << rules_applied <<"\n";
    assert(_stack.size() <= split_depth || job_no > 0);
    if (_stack.size() > last_depth && _stack.size() < 1000) {
      width[_stack.size()]++;
      if (_stack.size() <= clock_depth)
        std::cout << "^" << _stack.size() << ":" << clock() << "\n";
    }
    if (_stack.size() == split_depth && last_depth < split_depth) {
      if ( to_job(width[_stack.size()]-1) != (job_no-1) )   {
         while (_stack.size() >= split_depth) 
         	_rollback_to_latest_choice();
         //rules_applied = false;
         last_depth=_stack.size();
         //std::cout << "H" << hash << "," << job_no << "/" << num_of_job << std::endl;
         goto loop;
      }
    }
    last_depth=_stack.size();

    //last_depth=_stack.size();

    rules_applied = true;
    while (rules_applied) {
      rules_applied = false;

      if (__builtin_expect(frame.formulas.none(), 0)) {
        _state = State::PAUSED;
        _result = Result::SATISFIABLE;
        _loop_state = frame.chain->id;

		_print_stats();

        return _result;
      }

      if (_check_contradiction_rule()) {
        _rollback_to_latest_choice();
        ++_stats.total_frames;
        ++_stats.cross_by_contradiction;
        goto loop;
      }

      if (_apply_conjunction_rule())
        rules_applied = true;
      if (_apply_always_rule())
        rules_applied = true;

      if (_apply_disjunction_rule()) {
        Frame new_frame(frame);
        new_frame.formulas[_lhs[frame.choosen_formula]] = true;
        _stack.push(std::move(new_frame));

        ++_stats.total_frames;
        _stats.maximum_frames = std::max(
          _stats.maximum_frames, static_cast<uint64_t>(_stack.size()));

        goto loop;
      }

	  // TODO: Don't generate eventualities here at all
      if (_has_eventually && _apply_eventually_rule()) {
        auto &ev =
          frame.eventualities[_fw_eventualities_lut[_lhs[frame.choosen_formula]]];

        if (__builtin_expect(ev.is_not_requested(), 0))
          ev.set_not_satisfied();

        Frame new_frame(frame);
        new_frame.formulas[_lhs[frame.choosen_formula]] = true;
        _stack.push(std::move(new_frame));

        ++_stats.total_frames;
        _stats.maximum_frames = std::max(_stats.maximum_frames,
                                         static_cast<uint64_t>(_stack.size()));

        goto loop;
      }

      if (_has_until && _apply_until_rule())
	  {
        auto &ev =
          frame.eventualities[_fw_eventualities_lut[_rhs[frame.choosen_formula]]];
        if (__builtin_expect(ev.is_not_requested(), 0))
          ev.set_not_satisfied();

        Frame new_frame(frame);
        new_frame.formulas[_rhs[frame.choosen_formula]] = true;
        _stack.push(std::move(new_frame));

        ++_stats.total_frames;
        _stats.maximum_frames = std::max(_stats.maximum_frames,
                                         static_cast<uint64_t>(_stack.size()));

        goto loop;
      }

      if (_has_release && _apply_release_rule())
	  {
        Frame new_frame(frame);
        new_frame.formulas[_lhs[frame.choosen_formula]] = true;
        new_frame.formulas[_rhs[frame.choosen_formula]] = true;
        _stack.push(std::move(new_frame));

        ++_stats.total_frames;
        _stats.maximum_frames = std::max(_stats.maximum_frames,
                                         static_cast<uint64_t>(_stack.size()));

        goto loop;
      }

      if (rules_applied)
        goto loop;
    }

    _update_eventualities_satisfaction();
    _update_history();

    bool loop_result = false;

    std::tie(loop_result, _loop_state) = _check_loop_rule();
    if (loop_result)
	{
      _result = Result::SATISFIABLE;
      _state = State::PAUSED;

	  _print_stats();

      return _result;
    }

    if (_check_prune0_rule() || _check_prune_rule())
	{
      _rollback_to_latest_choice();
      ++_stats.total_frames;
      ++_stats.cross_by_prune;
      goto loop;
    }

    if (frame.id >= _maximum_depth)
	{
      _rollback_to_latest_choice();
      ++_stats.total_frames;
      goto loop;
    }

    Frame new_frame(frame.id + 1, _number_of_formulas, frame.eventualities,
                    &frame);
    _bitset.temporary = frame.formulas;
    _bitset.temporary &= _bitset.tomorrow;

    for (uint64_t i = 0; i < _number_of_formulas; ++i) {
      if (_bitset.temporary[i]) {
        assert(frame.formulas[i]);
        assert(_bitset.tomorrow[i]);
        new_frame.formulas[_lhs[i]] = true;
      }
    }

    frame.type = Frame::STEP;
    _stack.push(std::move(new_frame));

    ++_stats.total_frames;

    _stats.maximum_model_size = std::max(
      _stats.maximum_model_size, static_cast<uint64_t>(_stack.top().id));
  }

  _state = State::DONE;
  if (_result == Result::UNDEFINED)
    _result = Result::UNSATISFIABLE;

  _print_stats();
  
  

  return _result;
}

void Solver::_update_eventualities_satisfaction()
{
  Frame &frame = _stack.top();

  uint64_t i = 0;
  std::for_each(frame.eventualities.begin(), frame.eventualities.end(),
                [&](Eventuality &ev) {
                  if (frame.formulas[_bw_eventualities_lut[i]])
                    ev.set_satisfied(frame.id);
                  ++i;
                });
}

void Solver::_update_history()
{
  Frame *current_frame = _stack.top().chain;
  Frame &top_frame = _stack.top();

  while (current_frame) {
    if (current_frame->formulas == top_frame.formulas) {
      top_frame.prev = current_frame;
      top_frame.first = current_frame->first;
      return;
    }

    current_frame = current_frame->chain;
  }

  top_frame.prev = &top_frame;
  top_frame.first = &top_frame;
}

std::pair<bool, FrameID> Solver::_check_loop_rule() const
{
  const Frame &top_frame = _stack.top();
  const FrameID first_frame_id = top_frame.first->id;

  if (top_frame.first == &top_frame)
    return std::make_pair(false, FrameID(0));

  bool ret =
    std::all_of(top_frame.eventualities.begin(), top_frame.eventualities.end(),
                [first_frame_id](Eventuality ev) {
                  return ev.is_not_requested() ||
                         (ev.is_satisfied() && ev.id() > first_frame_id);
                });

  return std::make_pair(ret, first_frame_id);
}

bool Solver::_check_prune0_rule() const
{
  const Frame &top_frame = _stack.top();
  const FrameID prev_frame_id = top_frame.prev->id;

  if (top_frame.prev == &top_frame)
    return false;

  return !top_frame.eventualities.empty() &&
         std::none_of(top_frame.eventualities.begin(),
                      top_frame.eventualities.end(),
                      [&, prev_frame_id](Eventuality ev) {
                        if (ev.is_not_requested() || ev.is_not_satisfied())
                          return false;

                        return ev.id() > prev_frame_id;
                      });
}

bool Solver::_check_prune_rule() const
{
  const Frame &top_frame = _stack.top();

  if (top_frame.prev == top_frame.first)
    return false;

  uint64_t i = 0;
  return std::all_of(
    top_frame.eventualities.begin(), top_frame.eventualities.end(),
    [&top_frame, &i](Eventuality ev) {
      if (ev.is_not_requested() || ev.is_not_satisfied() ||
          ev.id() <= top_frame.prev->id) {
        ++i;
        return true;
      }

      bool ret = top_frame.prev->eventualities[i].is_satisfied() &&
                 top_frame.prev->eventualities[i].id() > top_frame.first->id;
      ++i;
      return ret;
    });
}

// This is probably not updating the solver stats correctly (what happens when
// we pop a STEP frame?)
void Solver::_rollback_to_latest_choice()
{
  while (!_stack.empty()) {
    if (_stack.top().type == Frame::CHOICE &&
        _stack.top().choosen_formula != FormulaID::max()) {
      Frame &top = _stack.top();
      ;
      Frame new_frame(top);

      if (_bitset.disjunction[top.choosen_formula])
        new_frame.formulas[_rhs[top.choosen_formula]] = true;
      else if (_bitset.eventually[top.choosen_formula]) {
        new_frame.formulas[top.choosen_formula + 1] = true;
        assert(_bitset.tomorrow[top.choosen_formula + 1] &&
               _lhs[top.choosen_formula + 1] == top.choosen_formula);
      }
      else if (_bitset.until[top.choosen_formula]) {
        new_frame.formulas[_lhs[top.choosen_formula]] = true;
        if (_bitset.tomorrow[top.choosen_formula + 1]) {
          new_frame.formulas[top.choosen_formula + 1] = true;
          assert(_lhs[top.choosen_formula + 1] == top.choosen_formula);
        }
        else {
          new_frame.formulas[top.choosen_formula + 2] = true;
          assert(_lhs[top.choosen_formula + 2] == top.choosen_formula);
        }
      }
      else if (_bitset.release[top.choosen_formula])
      {
        new_frame.formulas[_rhs[top.choosen_formula]] = true;
        if (_bitset.tomorrow[top.choosen_formula + 1]) {
			if (_lhs[top.choosen_formula + 1] == top.choosen_formula)
				new_frame.formulas[top.choosen_formula + 1] = true;
			else
				new_frame.formulas[top.choosen_formula + 2] = true;
        }
        else {
          new_frame.formulas[top.choosen_formula + 2] = true;
          assert(_lhs[top.choosen_formula + 2] == top.choosen_formula);
        }
      }
      else
        assert(false);

      top.choosen_formula = FormulaID::max();
      _stack.push(std::move(new_frame));

      return;
    }

    _stack.pop();
  }
}

ModelPtr Solver::model()
{
  if (_state != State::PAUSED)
    return nullptr;

  if (_result == Result::UNSATISFIABLE || _result == Result::UNDEFINED)
    return nullptr;

  ModelPtr model = std::make_shared<Model>();

  if (_subformulas.size() == 1 && isa<True>(_subformulas[0])) {
    model->loop_state = 0;
    model->states.push_back({Literal(u8"\u22a4")});
    return model;
  }

  uint64_t i = 0;
  for (const auto &frame : Container(_stack)) {
    if (frame.type == Frame::CHOICE)
      continue;

    LTL::detail::State state;
    for (uint64_t j = 0; j < _number_of_formulas; ++j) {
      if (frame.formulas[j]) {
        if (_atom_set.find(FormulaID(j)) != _atom_set.end())
          state.insert(Literal(_atom_set.find(FormulaID(j))->second));
        else if (_bitset.negation[j] &&
                 _atom_set.find(_lhs[j]) != _atom_set.end())
          state.insert(
            Literal(_atom_set.find(FormulaID(_lhs[j]))->second, false));
      }
    }

    model->states.push_back(state);
    ++i;
  }

  if (_stack.top().id != 0)
    model->states.pop_back();
  model->loop_state = _loop_state;

  return model;
}

FormulaPtr inline Solver::Formula() const
{
  return _formula;
}

void Solver::_print_stats() const
{
	format::debug("Total frames: {}", _stats.total_frames);
	format::debug("Maximum model size: {}", _stats.maximum_model_size);
	format::debug("Maximum depth: {}", _stats.maximum_frames);
	format::debug("Cross by contradiction: {}",
				  _stats.cross_by_contradiction);
	format::debug("Cross by prune: {}", _stats.cross_by_prune);
	if (getenv("JOB_NO"))
		std::cout << "JOB_NO="<<getenv("JOB_NO")<<" ";
	std::cout << "WIDTH ";
	for(int i=1;i<1000&&width[i];++i) {
		std::cout << i << ":" << width[i] << " ";
	} 
        std::cout << "\n^0:" << clock() << std::endl;

	const char* job_no="";
	if (getenv("JOB_NO")) job_no=getenv("JOB_NO");
	std::stringstream stream;
	if (_result == Result::SATISFIABLE) {
		stream << "IsSat!" 
			<< " JOB="<<job_no<< " SEC=" << (double)clock()/1000000 << std::endl; 
	}
	if (_result == Result::UNSATISFIABLE) {
		stream << "Unsat!"
			<< " JOB="<<job_no<< " SEC=" << (double)clock()/1000000 << std::endl; 
	}
	std::cerr << stream.str();
    
}

void Solver::__dump_current_formulas() const
{
  PrettyPrinter p;
  for (uint64_t i = 0; i < _subformulas.size(); ++i)
    if (_stack.top().formulas[i])
      format::verbose("{}", p.to_string(_subformulas[i]));
}

void Solver::__dump_current_eventualities() const
{
  PrettyPrinter p;
  for (uint64_t i = 0; i < _bw_eventualities_lut.size(); ++i)
    format::verbose("{} : {}", _subformulas[_bw_eventualities_lut[i]],
                    static_cast<uint64_t>(_stack.top().eventualities[i].id()));
}

void Solver::__dump_eventualities(FrameID id) const
{
  PrettyPrinter p;
  Frame *current_frame = _stack.top().chain;
  while (current_frame) {
    if (current_frame->id == id)
      break;

    current_frame = current_frame->chain;
  }

  for (uint64_t i = 0; i < _bw_eventualities_lut.size(); ++i)
	  format::verbose(
		  "{} : {}", _subformulas[_bw_eventualities_lut[i]],
		  static_cast<uint64_t>(Container(_stack)[id].eventualities[i].id()));
}

static bool formula_ordering_func(const FormulaPtr& a, const FormulaPtr& b)
{
	if (isa<Atom>(a) && isa<Atom>(b))
		return std::lexicographical_compare(fast_cast<Atom>(a)->name().begin(),
											fast_cast<Atom>(a)->name().end(),
											fast_cast<Atom>(b)->name().begin(),
											fast_cast<Atom>(b)->name().end());

	if (isa<Negation>(a) && isa<Negation>(b))
		return formula_ordering_func(fast_cast<Negation>(a)->formula(),
									 fast_cast<Negation>(b)->formula());

	if (isa<Negation>(a))
	{
		if (fast_cast<Negation>(a)->formula() == b)
			return false;

		return formula_ordering_func(fast_cast<Negation>(a)->formula(), b);
	}

	if (isa<Negation>(b))
	{
		if (fast_cast<Negation>(b)->formula() == a)
			return true;

		return formula_ordering_func(a, fast_cast<Negation>(b)->formula());
	}

	if (isa<Tomorrow>(a) && isa<Tomorrow>(b))
		return formula_ordering_func(fast_cast<Tomorrow>(a)->formula(),
									 fast_cast<Tomorrow>(b)->formula());

	if (isa<Tomorrow>(a))
	{
		if (fast_cast<Tomorrow>(a)->formula() == b)
			return false;

		return formula_ordering_func(fast_cast<Tomorrow>(a)->formula(), b);
	}

	if (isa<Tomorrow>(b))
	{
		if (fast_cast<Tomorrow>(b)->formula() == a)
			return true;

		return formula_ordering_func(a, fast_cast<Tomorrow>(b)->formula());
	}

	if (isa<Always>(a) && isa<Always>(b))
		return formula_ordering_func(fast_cast<Always>(a)->formula(),
									 fast_cast<Always>(b)->formula());

	if (isa<Eventually>(a) && isa<Eventually>(b))
		return formula_ordering_func(fast_cast<Eventually>(a)->formula(),
									 fast_cast<Eventually>(b)->formula());

	if (isa<Conjunction>(a) && isa<Conjunction>(b))
	{
		if (fast_cast<Conjunction>(a)->left() !=
			fast_cast<Conjunction>(b)->left())
			return formula_ordering_func(fast_cast<Conjunction>(a)->left(),
										 fast_cast<Conjunction>(b)->left());
		else
			return formula_ordering_func(fast_cast<Conjunction>(a)->right(),
										 fast_cast<Conjunction>(b)->right());
	}

	if (isa<Disjunction>(a) && isa<Disjunction>(b))
	{
		if (fast_cast<Disjunction>(a)->left() !=
			fast_cast<Disjunction>(b)->left())
			return formula_ordering_func(fast_cast<Disjunction>(a)->left(),
										 fast_cast<Disjunction>(b)->left());
		else
			return formula_ordering_func(fast_cast<Disjunction>(a)->right(),
										 fast_cast<Disjunction>(b)->right());
	}

	if (isa<Until>(a) && isa<Until>(b))
	{
		if (fast_cast<Until>(a)->left() != fast_cast<Until>(b)->left())
			return formula_ordering_func(fast_cast<Until>(a)->left(),
										 fast_cast<Until>(b)->left());
		else
			return formula_ordering_func(fast_cast<Until>(a)->right(),
										 fast_cast<Until>(b)->right());
	}

	if (isa<Then>(a) || isa<Then>(b))
		assert(false);

	if (isa<Iff>(a) && isa<Iff>(b))
		assert(false);

	return a->type() < b->type();
}

}
}
