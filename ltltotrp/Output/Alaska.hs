module Output.Alaska (
  Alaska(..)
  ) where

import LTL
import Text.PrettyPrint

newtype Alaska a = Alaska a

instance ExportLTL Alaska where
  export (Alaska f) = render . pretty $ f

ppretty = parens . pretty

pretty :: LTL -> Doc
pretty  Truth         = "true"
pretty  Falsity       = "false"
pretty (Atom   s)     = text s
pretty (Neg    f)     = "!" <> ppretty f
pretty (Or     f1 f2) = ppretty f2 <+> "||"  <+> ppretty f2
pretty (And    f1 f2) = ppretty f1 <+> "&&"  <+> ppretty f2
pretty (Imply  f1 f2) = ppretty f1 <+> "->"  <+> ppretty f2
pretty (Iff    f1 f2) = ppretty f1 <+> "<->" <+> ppretty f2
pretty (Until  f1 f2) = ppretty f1 <+> "U"   <+> ppretty f2
pretty (Always f)     = "G" <> ppretty f
pretty (Future f)     = "F" <> ppretty f
pretty (Next   f)     = "X" <> ppretty f
