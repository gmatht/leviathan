module Main where

import System.Environment

import Data.Maybe    (fromMaybe)
import Control.Monad

import LTL
import Parser


main = do
  args <- getArgs
  case args of
    [] -> putStrLn "Please specify the input file"
    (file:_) -> do
      contents <- readFile file
      case parseFormula file contents of
        Left err -> print err
        Right formula -> print formula
