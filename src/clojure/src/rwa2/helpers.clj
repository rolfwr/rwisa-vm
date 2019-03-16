(ns rwa2.helpers)

(defn slurp-bytes [file]
  (with-open [out (java.io.ByteArrayOutputStream.)]
    (clojure.java.io/copy (clojure.java.io/input-stream file) out)
    (.toByteArray out)))

(defn file->byte-vec [file]
  (vec (slurp-bytes file)))
