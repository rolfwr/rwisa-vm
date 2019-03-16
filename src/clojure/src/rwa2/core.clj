(ns rwa2.core
  (:gen-class)
  (:require [rwa2.helpers :refer [file->byte-vec]]
            [rwa2.terminal :as terminal]))

(defn fetch-ptr [memory addr]
  (->> (range 4)
       (map (comp memory (partial + addr)))
       (map-indexed (fn [idx val] (bit-shift-left (bit-and val 0xFF) (* 8 idx))))
       (apply +)))

(defn instr-halt [state]
  (assoc state :done? true))

(defn instr-input [{:keys [pc memory term] :as state}]
  (let [dst-ptr (fetch-ptr memory (inc pc))
        key (terminal/read-byte term)]
    (terminal/put-byte term key)
    (-> state
        (assoc-in [:memory dst-ptr] key)
        (assoc-in [:pc] (+ 5 pc)))))

(defn instr-output [{:keys [pc memory term] :as state}]
  (->> (fetch-ptr memory (inc pc))
       (memory)
       (char)
       (terminal/put-byte term))
  (assoc state :pc (+ 5 pc)))

(defn instr-branch-if-plus [{:keys [pc memory] :as state}]
  (let [target-ptr (fetch-ptr memory (inc pc))
        src-ptr    (fetch-ptr memory (+ 5 pc))]
    (->> (if (neg? (memory src-ptr)) (+ 9 pc) target-ptr)
         (assoc state :pc))))

(defn instr-subtract [{:keys [pc memory] :as state}]
  (let [dst-ptr (fetch-ptr memory (inc pc))
        src-ptr (fetch-ptr memory (+ 5 pc))
        new-val (unchecked-byte (- (memory dst-ptr) (memory src-ptr)))]
    (-> state
        (assoc-in [:memory dst-ptr] new-val)
        (assoc-in [:pc] (+ 9 pc)))))

(defn run-step [{:keys [pc memory] :as state}]
  (let [op (case (memory pc)
             0x0 instr-halt
             0x1 instr-output
             0x2 instr-branch-if-plus
             0x3 instr-subtract
             0x4 instr-input)]
    (op state)))

(defn ->vm [program terminal]
  {:pc 0
   :term terminal
   :memory program
   :done? false})

(defn run-vm [{:keys [term] :as state}]
  (terminal/start term)
  (loop [state state]
    (if-not (:done? state)
      (recur (run-step state))
      (do (terminal/stop term)
          0))))

(defn -main
  [filename & args]
  (let [term (terminal/->lanterna-terminal)] ;; or use terminal/->write-only-terminal
    (-> (file->byte-vec filename)
        (->vm term)
        (run-vm))))

;; (run-vm (->vm (file->byte-vec "rwa2/3-fizz.rwa2")
;;               (terminal/->lanterna-terminal :unix)))
