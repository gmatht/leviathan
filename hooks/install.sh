#!/bin/sh

hooks=pre-commit
for hook in $hooks; do
  cp -v $hook ../.git/hooks/
done
