{-# LANGUAGE RankNTypes, ImpredicativeTypes #-}

module Main where

import System.Environment
import System.Console.GetOpt

import Data.Either
import Data.Maybe    (fromMaybe)
import Control.Monad

import LTL
import Parser
import Output.Alaska


data Flag = Help
          | ToTRP
          | ToAlaska

defaultOptions :: Flag
defaultOptions = ToAlaska

options :: [OptDescr (Flag -> Flag)]
options = [
  Option ['h'] ["help"]   (NoArg $ const Help) "Show this help message",
  Option []    ["trp"]    (NoArg $ const ToTRP) "Output in TRP++ format",
  Option []    ["alaska"] (NoArg $ const ToAlaska)
    "Output in Alaska format (default)"
 ]

usage :: String
usage = usageInfo header options
  where header = "Usage: ltltotrp [options] file"

parseArgs :: [String] -> IO (Flag,[String])
parseArgs args =
  case getOpt Permute options args of
    (o,n,[])   -> return $ (foldl (flip ($)) defaultOptions o,n)
    (_,_,errs) -> ioError . userError $ concat errs ++ usage


main = do
  args <- getArgs
  (flag, files) <- parseArgs args
  formulae <- forM files $ \file -> do
    content <- readFile file
    case parseFormula file content of
      Left err ->
        ioError . userError $ "Parsing error in " ++ file ++ ": " ++ show err
      Right formula -> return formula
  case flag of
    Help -> putStrLn usage
    ToAlaska -> mapM_ (putStrLn . export . Alaska) formulae
    ToTRP -> putStrLn "TRP unimplemented"

