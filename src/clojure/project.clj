(defproject rwa2 "0.1.0-SNAPSHOT"
  :description "RWISA (rwa2) VM in Clojure"
  :url "http://example.com/FIXME"
  :license {:name "EPL-2.0 OR GPL-2.0-or-later WITH Classpath-exception-2.0"
            :url "https://www.eclipse.org/legal/epl-2.0/"}
  :dependencies [[org.clojure/clojure "1.9.0"]
                 [clojure-lanterna "0.9.7"]]
  :main rwa2.core
  :repl-options {:init-ns rwa2.core})
