#!/bin/bash
echo "# XSCALAAPPS" >> README.md
git init
git add README.md
git commit -m "XSCALA APPLICATIONS first commit"
git remote add origin git@github.com:urids/XSCALAAPPS.git
git push -u origin master
