(ns avi.command-line-mode-test
  (:require [midje.sweet :refer :all]
            [avi.test-helpers :refer :all]))

(facts "regarding command-line mode"
  (fact "`:` echos on the command-line"
    (editor :after ":")
     => (looks-like
          "One                 "
          "Two                 "
          "Three               "
          ".                   "
          "~                   " [:blue]
          "~                   " [:blue]
          "test.txt            " [:black :on :white]
          ":                   "))
  (fact "`:` places the cursor after the colon prompt"
    (cursor :after ":") => [7 1])
  (fact "characters typed after `:` echo on the command-line"
    (editor :after ":abc")
     => (looks-like
          "One                 "
          "Two                 "
          "Three               "
          ".                   "
          "~                   " [:blue]
          "~                   " [:blue]
          "test.txt            " [:black :on :white]
          ":abc                "))
  (fact "characters typed after `:` move the cursor"
    (cursor :after ":a") => [7 2]
    (cursor :after ":abc") => [7 4]))

(facts "regarding `:q`"
  (fact "Avi doesn't start in the 'finished' state"
    (:mode (editor)) =not=> :finished)
  (fact "Typing part of `:q<Enter>` doesn't exit Avi"
    (:mode (editor :after ":")) =not=> :finished
    (:mode (editor :after ":q")) =not=> :finished)
  (fact "`:q<Enter>` exits Avi."
    (:mode (editor :after ":q<Enter>")) => :finished))