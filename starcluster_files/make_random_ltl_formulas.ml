#!/usr/bin/env ocaml
(* parameters: size1 num_atoms interation_start iteration_end *)
(* based on make_random_ctls_formulas.ml from TimeLogicUnify *)
let printf = Printf.printf
let s = string_of_int
let infix op x y = "( " ^ x ^ " " ^ op ^ " " ^ y ^ " )"
let uni y x  = y ^ " " ^x
let array_to_string  = fun j f a -> String.concat j (Array.to_list (Array.map  f a)) 

let f_unimodal = [| uni "~"; uni "X"; uni "F"; uni "G"  |]
let f_bimodal = [| infix "U"; infix "&"; infix "|"  |]

let get_argv_int n default =
    if (Array.length (Sys.argv)) > n 
    then int_of_string Sys.argv.(n) 
    else default 

let iteration_start = get_argv_int 3 0 
let iteration_end = get_argv_int 4 iteration_start 
let size1 = get_argv_int 1 40 ;;
let num_atoms = get_argv_int 2 1 ;;
let rec range i j = if i > j then [] else i :: (range (i+1) j)
let atoms = Array.of_list (List.map (fun i -> Char.escaped (Char.chr (i + (Char.code 'a')))) (range 0 (num_atoms-1)))

let rand_string bi uni atoms size =
        let _ = "" ^ (uni.(0) "") in
        let _ = "" ^ (bi.(0) "" "") in
        let uni_l = Array.length uni in
        let atoms_l = Array.length atoms in
        let bi_l  = Array.length bi in
        let rec ff size = "" ^
              match (0+size) with 
              1 -> "" ^ atoms.(Random.int atoms_l)  
              | 2 -> "" ^ uni.(Random.int uni_l) (ff(size-1))
              | _ -> "" ^ match Random.bool () with
                        true ->  uni.(Random.int uni_l) (ff(size-1))
                        | false -> let split = Random.int (size-2) in
                                let x = ff (split+1) in
                                let y = ff ((size-split)-2) in
                                "" ^ bi.(Random.int bi_l) x y
        in
        ff (size+0);;
           
let rand_formula = rand_string f_bimodal f_unimodal;;

for i = iteration_start to iteration_end do
        let _ = Random.full_init [|i|] in
        printf "%s\n" (rand_formula atoms size1) 
done
