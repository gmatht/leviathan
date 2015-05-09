module Parser (
  parseFormula
) where


import LTL

import Data.Functor
import Control.Monad.Identity

import Text.Parsec
import Text.Parsec.Expr
import qualified Text.Parsec.Token as T


type Parser a = ParsecT String () Identity a

--
-- Entry point
--
parseFormula :: SourceName -> String -> Either ParseError LTL
parseFormula = parse formula

--
-- Auxiliary functions
--
langDef :: Monad m => T.GenLanguageDef String () m
langDef = T.LanguageDef {
  T.commentStart    = "",
  T.commentEnd      = "",
  T.commentLine     = "#",
  T.nestedComments  = False,
  T.identStart      = letter,
  T.identLetter     = alphaNum,
  T.opStart         = undefined,
  T.opLetter        = undefined,
  T.reservedNames   = [],
  T.reservedOpNames = [],
  T.caseSensitive   = False
}

lexer :: T.GenTokenParser String () Identity
lexer = T.makeTokenParser langDef

keyw :: String -> Parser String
keyw = T.lexeme lexer . string

tok :: [String] -> Parser ()
tok []  = return ()
tok ops = (void . choice . map (try . keyw) $ ops)
       <?> (head ops ++ " operator")

parens :: Parser a -> Parser a
parens = T.parens lexer


--
-- Grammar
--
atom :: Parser LTL
atom = Atom <$> T.identifier lexer

true :: Parser LTL
true = do
  tok ["TRUE", "true"]
  return Truth

false :: Parser LTL
false = do
  tok ["FALSE", "false"]
  return Falsity

neg_op    :: Parser ()
or_op     :: Parser ()
and_op    :: Parser ()
imply_op  :: Parser ()
iff_op    :: Parser ()
until_op  :: Parser ()
always_op :: Parser ()
future_op :: Parser ()
next_op   :: Parser ()

neg_op    = tok ["NOT",  "!",   "~"]
or_op     = tok ["OR",   "||",  "|"]
and_op    = tok ["AND",  "&&",  "&"]
imply_op  = tok ["THEN", "->"      ]
iff_op    = tok ["IFF",  "<->", "="]
until_op  = tok ["U"]
always_op = tok ["G",    "[]"      ]
future_op = tok ["F",    "<>"      ]
next_op   = tok ["X"]


formula :: Parser LTL
formula = buildExpressionParser table term
       <?> "LTL formula"

term :: Parser LTL
term    =  parens formula
       <|> atom
       <?> "Simple expression"

table :: OperatorTable String () Identity LTL
table = [
  [prefix neg_op    Neg   ],
  [prefix next_op   Next,
   prefix future_op Future,
   prefix always_op Always],
  [binary and_op    And   ],
  [binary or_op     Or    ],
  [binary imply_op  Imply ],
  [binary iff_op    Iff   ],
  [binary until_op  Until ]
 ]

binary  prod f = Infix   (prod >> return f) AssocLeft
prefix  prod f = Prefix  (prod >> return f)

