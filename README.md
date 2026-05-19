# 3D-FACE-classification

**Script files for data preparation**

All files were processed with imagexd ([https://stark-jena.de/](https://stark-jena.de/)).

[Imagexd](https://stark-jena.de/research-interests/software/imagexd/)

[see more](https://stark-jena.de/current-responsibilities/3d-face/)

---

* best.macro // Calculate the best (min two raters) MRI dataset
* best_x3.macro // Calculate the bestx3 (min three raters) MRI dataset

* repair_transformation.macro // some rater use different coordinate systems -> all to rater HS

* rater.macro // generate statistics data for raters'
* thickness.macro // generate statistics data for muscle thickness
* distance.macro // generate statistics data for bone - muscle distances

* stat.macro // statistics for muscle groups
* stat_rater.macro // statistics for rater classification of muscle groups
* stat_*_sub.macro // statistics for mimic muscles divided into 16 parts

* mrt_orientation.inc // helper
* mrt_orientation_checkup.inc // helper

---

* imagexd.macro // imagexd command overview
