(ns rwa2.terminal
  (:require [lanterna.terminal :as t]))

(defprotocol ITerminal
  "Turns out it's stupid hard to read single chars blocking
  while developing using a REPL, as the REPL owns STDIN.
  You can easily read lines, but not single key presses.

  I used a UI terminal to avoid not spending time on this issue.
  Implement this protocol if you have different terminal needs..
  "
  (start [this])
  (stop [this])
  (put-byte [this b])
  (read-byte [this]))

(defn ->lanterna-terminal
  "Lanterna based terminal"
  ([] (->lanterna-terminal :unix)) ; or :swing
  ([termtype]
   (let [term (t/get-terminal termtype)]
     (reify ITerminal
       (start [_] (t/start term))
       (stop [_] (do
                   (t/put-string term "\nPress any key to close..")
                   (t/get-key-blocking term)
                   (t/stop term)))
       (put-byte [_ b] (t/put-character term (char b)))
       (read-byte [_] (let [key (t/get-key-blocking term)]
                        (-> (get {:enter (byte \newline)} key key)
                            (int)
                            (unchecked-byte))))))))

(defn ->write-only-terminal
  "For running non-interactive, only provides print. Return 42 when read.."
  []
  (reify ITerminal
    (start [_] nil)
    (stop [_] nil)
    (put-byte [_ b] (do (print (char b))))
    (read-byte [_] (byte 42))))
