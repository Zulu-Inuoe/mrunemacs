# mrunemacs

## Overview

Simple runner over emacs to make it behave a little bit better on Windows.

What it does:

1. Ensures the HOME variable is set. If it's not already set, it'll be set to USERPROFILE
2. If run with no args, will try try to find an emacs frame. If there is none, it'll launch emacs
3. When run with arguments, it passes these as files to emacs.
  It'll launch emacsclient with -n -a "", which makes emacs bring up the chosen file(s) in a buffer without creating a new frame
4. Whether with args or without, it'll try and find an emacs frame, and bring it to focus (restore if minimized, and raise)


This fits my desired use of emacs which is as a 'single instance' mode in some other editors. Meaning there's only one main emacs frame.
