(ns avi.core
  (:import [avi.terminal Screen])
  (:require [avi.buffer :as b]
            [avi.editor :as e]
            [avi.command-line-mode]
            [avi.normal-mode]
            [avi.insert-mode]
            [avi.render :as render])
  (:gen-class))

(defn start
  [[lines columns] & [filename]]
  {:mode :normal
   :buffer (b/open filename (- lines 2))
   :size [lines columns]
   :count nil
   :beep? false})

(defmethod e/process :resize
  [editor [_ size]]
  (-> editor
      (assoc :size size)
      (e/update-current-buffer #(b/resize % (- (first size) 2)))))

(defn- update-screen
  [editor]
  (let [{chars :chars,
         attrs :attrs,
         width :width,
         [i j] :cursor} (render/render editor)]
    (Screen/refresh i j width chars attrs)))

(defn- screen-size
  []
  (let [size (Screen/size)]
    [(get size 0) (get size 1)]))

(defn -main
  [& args]
  (Screen/start)
  (loop [[height width] (screen-size)
         editor (apply start [height width] args)]
    (if (:beep? editor)
      (Screen/beep))
    (let [editor (if (not= [height width] (:size editor))
                   (e/process editor [:resize [height width]])
                   editor)]
      (update-screen editor)
      (if-not (= (:mode editor) :finished)
        (recur
          (screen-size)
          (e/process editor [:keystroke (Screen/getKey)])))))
  (Screen/stop))
