module LTL where

data LTL = Truth
         | Falsity
         | Atom   String
         | Neg    LTL
         | Or     LTL LTL
         | And    LTL LTL
         | Imply  LTL LTL
         | Iff    LTL LTL
         | Until  LTL LTL
         | Always LTL
         | Future LTL
         | Next   LTL
  deriving (Eq, Show)


class ExportLTL w where
  export :: w LTL -> String
