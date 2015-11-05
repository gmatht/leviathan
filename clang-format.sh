#!/bin/bash

usage() {
  cat <<EOF >&2
Usage:
  clang-format.sh -- Help running clang-format on the repository

  clang-format.sh check [commit]
      Print the list of C++ files that are not correctly formatted according to
      clang-format. If the 'commit' argument is present, only files staged for
      commit on git are checked.

  clang-format.sh format [commit]
      Call clang-format to reformat C++ files in the repository. If the
      'commit' argument is present, only files staged for commit on git are 
      formatted.
EOF
}

install-hooks() {
  cat <<'EOF' > ./.git/hooks/pre-commit
#!/bin/bash

# Redirect output to stderr.
exec 1>&2

indent() { sed 's/^/    /'; }

changes=$(./clang-format.sh check commit)

if [ ! -z "$changes" ]; then
  echo -n "The following C++ files are not correctly formatted according "
  echo    "to clang-format:"
  
  for f in $changes; do
    echo $f | indent
  done
  
  echo
  echo -n "To format the files, run './clang-format.sh format commit' "
  echo    "and redo the commit"
  echo -n "If you want to force the commit anyway, use "
  echo    "'git commit --no-verify'"
  exit 1
fi
EOF
  chmod +x ./.git/hooks/pre-commit
  echo "Git pre-commit hook installed"
}

case "$1" in
  "check" | "format")
    
    cpp() {
      grep -E '\.(h|hpp|cpp)$' | grep -v 'external/'
    }

    #
    # If we have to check for files ready to be committed, ask to git the list,
    # otherwise find all C++ files in the filesystem
    #
    if [ "$2" = "commit" ]; then
      files=$(git diff --cached --name-only --diff-filter=ACM | cpp)
    else
      files=$(find . | cpp)
    fi

    case "$1" in
      "check")
        for f in $files; do
          changes=$(clang-format --style=file $f | diff $f -)
          if [ ! -z "$changes" ]; then
            echo $f
          fi
        done
      ;;
      "format")
        for f in $files; do
          clang-format -i --style=file $f
        done
      ;;
    esac
  ;;

  "install-hooks")
    if [ ! -e ".git" ]; then
      echo "This is not the top-level directory of a git repository"
      exit 1
    fi

    install-hooks
  ;;

  "usage")
    usage
  ;;

  *)
    if [ -z "$1" ]; then
      echo "Arguments missing" >&2  
    else
      echo "Unrecognized '$1' argument" >&2
    fi

    echo
    usage
    exit 1
  ;;
esac

