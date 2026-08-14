# 3D-FACE-Classification

**Advanced Analysis and Classification of Facial and Mimic Muscle Anatomy using 3D MRI Data**

A comprehensive collection of processing and analysis scripts for automated segmentation, classification, and quantitative characterization of facial anatomy, muscle groups, and bone structures from 3D MRI data.

## Overview

**3D-FACE-Classification** is a specialized research project focused on the detailed analysis, segmentation, and classification of facial anatomy from high-resolution 3D MRI (Magnetic Resonance Imaging) datasets. The project provides an automated pipeline for:

- **Multi-rater segmentation consensus** - Combining independent expert annotations
- **Muscle group classification** - Categorizing facial musculature into functional groups
- **Quantitative anatomy** - Measuring tissue thickness, distribution, and spatial relationships
- **Anatomical variation analysis** - Understanding differences across subjects
- **Biomechanical characterization** - Analyzing muscle organization for functional understanding

This project combines medical image analysis, consensus methodology, statistical analysis, and detailed anatomical classification to create comprehensive 3D models of facial anatomy.

## Project Context

### Facial Anatomy Research

Understanding facial anatomy is essential for:
- **Medical research** - Understanding facial function and dysfunction
- **Surgical planning** - Precise anatomical knowledge for safe intervention
- **Dermatology** - Muscle-related aging and aging-related changes
- **Orthodontics** - Masticatory muscle organization and function
- **Speech-language pathology** - Understanding mimic and speech-related muscles
- **Cosmetic medicine** - Evidence-based approach to facial aesthetics

### MRI Technology in Facial Imaging

3D MRI provides:
- **High soft tissue contrast** - Excellent muscle-fat differentiation
- **Non-invasive imaging** - No radiation exposure
- **High resolution** - Sub-millimeter spatial resolution achievable
- **3D volumetric data** - Complete spatial information
- **In vivo imaging** - Analysis of living subjects
- **Reproducibility** - Consistent image acquisition protocols

### Multi-Rater Consensus Approach

The project implements best-practices for handling multiple expert annotations:
- **Rater agreement analysis** - Assessing reproducibility and consistency
- **Consensus building** - Combining ratings from multiple experts
- **Inter-rater reliability** - Statistical measures of agreement
- **Quality control** - Identifying and managing discrepancies
- **Anatomical validation** - Cross-referencing with established anatomy

## Repository Contents

### Overview of Processing Pipeline

```
Raw 3D MRI Data
        ↓
[mrt_orientation.inc] - Coordinate system alignment & landmark detection
        ↓
[repair_transformation.macro] - Standardize coordinate systems across raters
        ↓
[rater.macro] - Analyze individual rater segmentations
        ↓
[best.macro] - Create consensus from 2+ raters
[best_x3.macro] - Create consensus from 3+ raters
        ↓
[thickness.macro] - Calculate muscle thickness maps
[distance.macro] - Calculate bone-muscle distance maps
        ↓
[stat_*.macro] - Statistical analysis by anatomical region
[stat_*_sub.macro] - Statistical analysis subdivided into 16 regions
        ↓
Anatomical Classification & Results
```

### Script Categories

#### A. Data Preparation & Consensus Building

##### 1. **mrt_orientation.inc** - MRI Coordinate System Alignment

**Purpose:** Standardizes MRI coordinate systems and detects anatomical landmarks for consistent spatial orientation.

**Key Functions:**
- Loads bone segmentation in original coordinate system
- Detects anatomical landmarks from marker files (.mrk.json format):
  - Eye landmarks (right eye upper/lower)
  - Anatomical reference points
  - Multiple marker file format support (fallback chain)
- Applies coordinate transformations:
  - Coordinate system inversion
  - Global-to-local coordinate conversion
  - Scaling adjustments for isotropic spacing
- Establishes reference coordinate system for all subsequent analyses

**Input Files:**
- Bone segmentation files (NIFTI format)
- Anatomical marker files (JSON format)
  - `*_1.mrk.json`, `*_auge rechts oben.mrk.json` (right eye upper)
  - `*_2.mrk.json`, `*_auge rechts unten.mrk.json` (right eye lower)
  - Additional anatomical markers as needed

**Output:**
- Standardized coordinate system definition
- Transformation matrices
- Landmark coordinates in standard space

**Technical Details:**
```
Coordinate Transformation:
1. Apply reflection: <-1, -1, 1> (mirror across specific axes)
2. Convert global to local coordinates: scalar.g2l
3. Apply scaling: <2, 2, 2> (achieve isotropic spacing)
```

##### 2. **repair_transformation.macro** - Coordinate System Standardization

**Purpose:** Repairs inconsistencies in coordinate systems used by different raters and standardizes all data to a common reference system.

**Key Operations:**
- Identifies coordinate system deviations
- Applies correction transformations
- Standardizes all rater segmentations to a single reference (rater HS - Heiko Stark)
- Validates transformation accuracy
- Ensures inter-rater data compatibility

**Background:**
Different raters may use different MRI coordinate system conventions. This script identifies and corrects these differences, ensuring all segmentations are comparable.

**Input:** Individual rater segmentation files with varying coordinate systems
**Output:** Transformed segmentations in standardized reference frame

##### 3. **best.macro** - Binary Consensus (2+ Raters)

**Purpose:** Creates a consensus muscle segmentation combining annotations from a minimum of 2 independent raters.

**Key Operations:**
- Loads all mask files for a subject with file naming convention `mask*.nii.gz`
- Excludes derived files:
  - `mask_sub.nii.gz` (subdivision masks)
  - `mask_XX.nii.gz` (consensus result)
  - `mask_X3.nii.gz` (3-rater consensus)
  - `mask_*split.nii.gz` (split versions)
- Sums rater segmentations
- Thresholds to binary consensus (value ≥ 2)
- Saves consensus mask as `mask_XX.nii.gz`

**Algorithm:**
```
For each subject directory:
  1. Load all individual rater masks
  2. Sum masks: mask_total = mask_rater1 + mask_rater2 + ...
  3. Apply threshold: consensus = (mask_total ≥ 2) ? 1 : 0
  4. Save as mask_XX.nii.gz
```

**Input:** Individual rater segmentation files
**Output:** `mask_XX.nii.gz` - Binary consensus mask

**File Naming Convention:**
- Individual raters: `mask_HS.nii.gz`, `mask_KS.nii.gz`, `mask_DS.nii.gz`, etc.
- 2+ consensus: `mask_XX.nii.gz`
- 3+ consensus: `mask_X3.nii.gz`

##### 4. **best_x3.macro** - High-Confidence Consensus (3+ Raters)

**Purpose:** Creates a more stringent consensus requiring agreement from 3 or more independent raters.

**Key Operations:**
- Similar to `best.macro` but requires minimum 3 raters
- Generates `mask_X3.nii.gz` with higher confidence
- Useful for controversial anatomical regions
- Better for validation and benchmark datasets

**Algorithm:**
```
Consensus = (mask_total ≥ 3) ? 1 : 0
```

**Use Cases:**
- Creating high-quality reference/gold-standard segmentations
- Validating algorithm performance
- Analyzing agreement patterns
- Identifying anatomically ambiguous regions

#### B. Muscle & Tissue Characterization

##### 5. **thickness.macro** - Muscle Thickness Calculation

**Purpose:** Computes local muscle thickness at each voxel using distance transform analysis.

**Key Operations:**
- Loads consensus muscle mask (`mask_XX.nii.gz`)
- Sets maximum scan length: 60 mm (thickness radius)
- Computes distance transform via local thickness measurement
- Applies fullscan=false for optimization (incomplete sphere scans acceptable)
- Saves thickness map: `Thickness/{ID}_thickness.nii.gz`
- Enables parallel processing (thread.max := 40)

**Thickness Analysis:**
- **Local thickness (LT):** Perpendicular distance to nearest muscle boundary
- **Medial axis:** Central axis through muscle structure
- **Thickness distribution:** Histogram of thickness values
- **Mean thickness:** Average across muscle region
- **Thickness variation:** Spatial variation of muscle thickness

**Input:** Consensus muscle segmentation
**Output:** 
- 3D thickness map showing local muscle thickness at each point
- Unit: millimeters
- Range: 0-60 mm (maximum scan radius)

**Interpretation:**
- **Low values (0-5 mm):** Thin muscle tissue at boundaries
- **Medium values (5-15 mm):** Typical mimic muscle thickness
- **High values (15-60 mm):** Thick muscle bundles or muscle groups
- **Zero values:** Outside muscle region

##### 6. **distance.macro** - Bone-Muscle Distance Analysis

**Purpose:** Calculates the 3D spatial distance from muscle segmentations to bone surfaces, revealing muscle-to-bone relationships.

**Key Operations:**
- Computes distance from each muscle voxel to nearest bone boundary
- Generates distance field in 3D space
- Saves as distance maps for statistical analysis
- Reveals anatomical muscle organization relative to skeleton
- Important for understanding functional muscle groups

**Applications:**
- Analyzing muscle attachment sites
- Understanding muscle-bone relationships
- Identifying muscle layers and stratification
- Biomechanical significance of muscle positioning

**Output:** 3D distance field showing muscle proximity to bone surfaces

#### C. Statistical Analysis

The repository includes comprehensive statistical analysis scripts for multiple levels of anatomical classification:

##### 7. **stat.macro** - Muscle Group Statistics

**Purpose:** Generates statistical summaries for major muscle group classifications.

**Key Operations:**
- Loads individual rater classifications (`mask*.nii.gz`)
- Loads anatomical subdivision mask (`mask_sub.nii.gz`)
- Classifies voxels into muscle groups:
  1. **Unknown** - Unclassified tissue
  2. **Mimic muscles** - Facial expression muscles
  3. **Platysma** - Neck muscle extending into lower face
  4. **Masticatory muscles** - Muscles of mastication
  5. **Eye muscles** - Orbicularis and extraocular muscles
  6. **Tongue muscles** - Lingual musculature

**Statistical Output:**
- Count of voxels per muscle group per rater
- Generates CSV output: `stat.csv`
- Headers: ID, N_Unknown, N_Mimik, N_Platysma, N_Mastication, N_Eye, N_Tongue

**Input:** Rater segmentation files and subdivision mask
**Output:** CSV file with muscle group counts

##### 8. **stat_rater.macro** - Rater Agreement Analysis

**Purpose:** Analyzes inter-rater agreement and reliability metrics.

**Key Operations:**
- Compares individual rater segmentations against consensus
- Calculates voxel-wise agreement statistics:
  - Agreement count (value = 1)
  - Partial agreement (value = 2)
  - Disagreement (value = 3)
- Outputs agreement metrics per rater
- Generates: `stat_rater.csv`

**Output Metrics:**
- Voxel count for each agreement level
- Rater reliability indicators
- Consistency assessment

##### 9. **stat_*_sub.macro** - Regional Subdivision Analysis

**Purpose:** Provides fine-grained spatial analysis by subdividing muscles into 16 regional parts.

**Scripts:**
- `stat_sub.macro` - General subdivision statistics
- `stat_bone_sub.macro` - Bone structure analysis by region
- `stat_distance_sub.macro` - Distance field analysis by region
- `stat_fat_sub.macro` - Adipose (fat) tissue analysis
- `stat_fat_in_sub.macro` - Internal fat analysis
- `stat_fat_out_sub.macro` - External fat analysis
- `stat_fat_intensities_sub.macro` - Fat tissue MRI intensity analysis
- `stat_intensities_sub.macro` - Overall MRI intensity analysis
- `stat_thickness_sub.macro` - Thickness analysis by regional subdivision

**Regional Subdivision Scheme:**
The face is subdivided into 16 regions (4 x 4 grid or hierarchical subdivision):
- **Anterior/Posterior** division
- **Superior/Inferior** division
- **Left/Right** subdivision
- Regional mapping for spatial analysis

**Output:** Detailed statistics for each of 16 regions:
- Voxel counts
- Intensity statistics
- Thickness measurements
- Distance measurements
- Tissue composition

#### D. Helper Files & Documentation

##### 10. **mrt_orientation_checkup.inc** - Validation Helper

**Purpose:** Validates coordinate system alignment and transformation correctness.

**Key Operations:**
- Verifies landmark detection success
- Checks coordinate system transformations
- Validates geometric consistency
- Diagnostic output for troubleshooting

**Input:** Transformed anatomy data
**Output:** Validation report and diagnostic metrics

##### 11. **imagexd.macro** - Command Reference

Complete reference documentation for all imagexd macro language commands used throughout the project.

### File Organization

**Directory Structure:**
```
3D-FACE-classification/
├── Processing Scripts
│   ├── best.macro                      # Consensus generation
│   ├── best_x3.macro                   # High-confidence consensus
│   ├── repair_transformation.macro     # Coordinate standardization
│   ├── thickness.macro                 # Thickness calculation
│   └── distance.macro                  # Distance field computation
│
├── Analysis Scripts (Rater Level)
│   ├── rater.macro                     # Individual rater analysis
│   └── stat_rater.macro                # Rater agreement metrics
│
├── Analysis Scripts (Muscle Groups)
│   ├── stat.macro                      # Major muscle group statistics
│   └── stat_*_sub.macro                # Regional subdivision analysis
│       ├── stat_sub.macro
│       ├── stat_bone_sub.macro
│       ├── stat_distance_sub.macro
│       ├── stat_fat_sub.macro
│       ├── stat_fat_in_sub.macro
│       ├── stat_fat_out_sub.macro
│       ├── stat_fat_intensities_sub.macro
│       ├── stat_intensities_sub.macro
│       └── stat_thickness_sub.macro
│
├── Helper Files
│   ├── mrt_orientation.inc             # Coordinate alignment
│   ├── mrt_orientation_checkup.inc     # Validation
│   └── imagexd.macro                   # Command reference
│
└── LICENSE                             # GPL-3.0

```

**Input Data Structure:**
```
Subject_Data/
├── Subject_001/
│   ├── mask_HS.nii.gz                  # Rater 1 segmentation
│   ├── mask_KS.nii.gz                  # Rater 2 segmentation
│   ├── mask_DS.nii.gz                  # Rater 3 segmentation
│   ├── mask_XX.nii.gz                  # 2+ consensus (generated)
│   ├── mask_X3.nii.gz                  # 3+ consensus (generated)
│   ├── mask_sub.nii.gz                 # Anatomical subdivisions
│   ├── Bones/
│   │   └── Subject_001 Segmentation-Bones.nii.gz
│   ├── Thickness/
│   │   └── Subject_001_thickness.nii.gz (generated)
│   ├── Distance/
│   │   └── Subject_001_distance.nii.gz (generated)
│   └── Markers/
│       ├── F_1.mrk.json                # Landmark files
│       ├── F_2.mrk.json
│       └── ... (additional markers)
│
├── Subject_002/
│   └── (similar structure)
│
└── ...
```

**Output Files:**
```
Results/
├── stat.csv                            # Muscle group statistics
├── stat_rater.csv                      # Rater agreement
├── stat_*_sub.csv                      # Regional analysis results
├── Thickness/
│   ├── Subject_001_thickness.nii.gz
│   ├── Subject_002_thickness.nii.gz
│   └── ...
└── Distance/
    ├── Subject_001_distance.nii.gz
    ├── Subject_002_distance.nii.gz
    └── ...
```

## Technical Details

### Software Requirements

#### Primary Tool: imagexd

**Website:** https://stark-jena.de/research-interests/software/imagexd/

**Features Used:**
- 3D image I/O (NIFTI format with gzip compression)
- Scalar field operations (segmentation, masking)
- Distance transform algorithms
- Statistical analysis functions
- Landmark/marker file parsing (JSON)
- Directory and file batch processing
- Parallel multi-threaded computation
- Image alignment and transformation

#### File Formats

**NIFTI Format (.nii.gz):**
- Standard neuroimaging format
- Includes spatial metadata (voxel spacing, orientation)
- Gzip compressed for storage efficiency
- 3D volumetric data support
- Single or multi-channel support

**Marker Format (.mrk.json):**
- JSON-based anatomical landmark file
- Contains 3D coordinates of anatomical points
- Used for coordinate system alignment
- Compatible with common MRI software (ITK-SNAP, etc.)

**CSV Format:**
- Comma/tab-separated statistical results
- Headers with clear column descriptions
- Compatible with spreadsheet and statistical software

### Data Requirements

**MRI Specifications:**
- **Resolution:** Typically 1-2 mm isotropic voxels
- **Contrast:** T1-weighted or FLAIR sequences
- **Field of Strength:** 1.5T or higher
- **Coverage:** Complete facial region including neck
- **Quality:** High signal-to-noise ratio for accurate segmentation

**Segmentation Requirements:**
- Independent annotation by trained anatomists/radiologists
- Consistent anatomical definitions across raters
- Coverage of major facial muscle groups
- Bone segmentation for reference

**Marker/Landmark Requirements:**
- Precise anatomical point identification
- Consistent landmark placement across subjects
- Multiple marker file format support for flexibility

### Processing Configuration

**Threading:**
```
thread.max := 40
// Adjust based on available CPU cores
// Typical: cores_available - 2 (leave room for system processes)
```

**Thickness Measurement Parameters:**
```
scalar.fullscan false
// false: scan incomplete spheres (faster)
// true: scan only complete spheres (more accurate)

Maximum scan length: 60 mm
// Typical muscle thickness rarely exceeds 40-50 mm
```

**Distance Measurement:**
- Euclidean distance in 3D space
- Computed from each muscle voxel to nearest bone boundary
- Used for muscle layer/stratification analysis

## Usage Guide

### Prerequisites

1. **Install imagexd** from https://stark-jena.de/research-interests/software/imagexd/
2. **Prepare MRI data** with anatomical segmentations from multiple raters
3. **Create landmark files** with anatomical reference points
4. **Organize data** in standard directory structure

### Data Preparation

**Step 1: Organize Input Data**
```
Create directory structure with subject folders:
- Each folder contains rater segmentations: mask_HS.nii.gz, mask_KS.nii.gz, etc.
- Include bone segmentations: Bones/ subdirectory
- Place marker files in root of subject directory
```

**Step 2: Run Coordinate Alignment**
```bash
imagexd mrt_orientation.inc
# This validates and aligns coordinate systems
```

**Step 3: Repair Coordinate Systems (if needed)**
```bash
imagexd repair_transformation.macro
# Standardize all rater segmentations to common reference
```

### Main Processing Pipeline

**Step 1: Generate Consensus Segmentations**
```bash
# Create 2+ rater consensus
imagexd best.macro
# Generates: mask_XX.nii.gz

# Create 3+ rater consensus (optional, for higher confidence)
imagexd best_x3.macro
# Generates: mask_X3.nii.gz
```

**Step 2: Calculate Tissue Measurements**
```bash
# Calculate muscle thickness
imagexd thickness.macro
# Generates: Thickness/*.nii.gz

# Calculate bone-muscle distances
imagexd distance.macro
# Generates: Distance/*.nii.gz
```

**Step 3: Analyze Individual Raters**
```bash
# Generate rater-specific statistics
imagexd rater.macro
# Generates: stat_rater.csv
```

**Step 4: Statistical Analysis**

**General muscle group statistics:**
```bash
imagexd stat.macro
# Generates: stat.csv
# Output: voxel counts per muscle group
```

**Regional subdivision statistics:**
```bash
# General regional statistics
imagexd stat_sub.macro

# Bone structure by region
imagexd stat_bone_sub.macro

# Distance measurements by region
imagexd stat_distance_sub.macro

# Fat tissue analysis
imagexd stat_fat_sub.macro
imagexd stat_fat_in_sub.macro
imagexd stat_fat_out_sub.macro
imagexd stat_fat_intensities_sub.macro

# Intensity analysis
imagexd stat_intensities_sub.macro

# Thickness by region
imagexd stat_thickness_sub.macro
```

### Batch Processing

**Automated full pipeline:**

Create a shell script `process_all.sh`:
```bash
#!/bin/bash

# Define dataset directory
DATASET="/path/to/data"

cd "$DATASET"

# Step 1: Coordinate alignment
echo "Aligning coordinate systems..."
imagexd mrt_orientation.inc > orientation.log 2>&1

# Step 2: Repair transformations (if needed)
# imagexd repair_transformation.macro > repair.log 2>&1

# Step 3: Consensus generation
echo "Generating consensus segmentations..."
imagexd best.macro > consensus.log 2>&1
imagexd best_x3.macro > consensus_x3.log 2>&1

# Step 4: Tissue characterization
echo "Calculating tissue measurements..."
imagexd thickness.macro > thickness.log 2>&1
imagexd distance.macro > distance.log 2>&1

# Step 5: Statistical analysis
echo "Running statistical analysis..."
imagexd rater.macro > rater.log 2>&1
imagexd stat.macro > stat.log 2>&1
imagexd stat_sub.macro > stat_sub.log 2>&1
imagexd stat_bone_sub.macro > stat_bone_sub.log 2>&1
imagexd stat_distance_sub.macro > stat_distance_sub.log 2>&1
imagexd stat_fat_sub.macro > stat_fat_sub.log 2>&1
imagexd stat_fat_in_sub.macro > stat_fat_in_sub.log 2>&1
imagexd stat_fat_out_sub.macro > stat_fat_out_sub.log 2>&1
imagexd stat_fat_intensities_sub.macro > stat_fat_intensities_sub.log 2>&1
imagexd stat_intensities_sub.macro > stat_intensities_sub.log 2>&1
imagexd stat_thickness_sub.macro > stat_thickness_sub.log 2>&1

echo "Processing complete!"
```

Run with:
```bash
chmod +x process_all.sh
./process_all.sh
```

### Customization

**Modify threading for your system:**
```
Edit thickness.macro or other scripts:
thread.max := 40  # Change to your CPU core count
```

**Adjust thickness measurement parameters:**
```
Edit thickness.macro:
new := scalar.measure.thickness 60  # Change 60 to different max radius
scalar.fullscan false  # Change to true for complete spheres only
```

**Change output locations:**
Modify paths in individual scripts:
```
a := "Thickness/" / x "-" y " thickness.nii.gz"
// Change "Thickness/" to desired output directory
```

## Research Applications

### Clinical Applications

**Facial Pathology:**
- Understanding muscle degeneration in muscular dystrophies
- Analyzing muscle changes in facial nerve palsy
- Studying aging-related changes in facial musculature
- Assessment of post-surgical outcomes

**Cosmetic Medicine:**
- Objective measurements for botulinum toxin injection planning
- Assessment of muscle atrophy and rejuvenation
- Personalized treatment planning
- Outcome documentation

**Orthodontics & Orthognathics:**
- Understanding masticatory muscle organization
- Planning surgical interventions
- Functional analysis of jaw muscles
- Post-surgical follow-up

### Research Applications

**Anatomy & Variation:**
- Documenting normal anatomical variation
- Sex-based differences in facial anatomy
- Age-related changes in muscle structure
- Ethnic variation in facial anatomy

**Biomechanics:**
- Understanding forces generated by facial muscles
- Analyzing muscle fiber organization
- Modeling facial movement
- Functional analysis of muscle groups

**Image Processing & AI:**
- Algorithm validation and benchmarking
- Creating gold-standard segmentation datasets
- Training deep learning models
- Evaluating segmentation accuracy

**Educational Purposes:**
- Creating detailed 3D anatomical models
- Medical student training
- Surgical planning and education
- Anatomical visualization

### Athlete & Performance Analysis

**Facial Expression:**
- Understanding smile mechanics
- Analyzing expression production
- Studying speech-related muscles
- Performance analysis for actors/athletes

## Output Data Interpretation

### Thickness Maps

**Interpretation:**
- **Units:** Millimeters
- **Range:** 0-60 mm (based on measurement parameters)
- **Low values (0-5 mm):** Muscle tissue at boundaries, thin muscle layers
- **Medium values (5-15 mm):** Typical mimic muscle thickness
- **High values (15-60 mm):** Thick muscle bundles, muscle groups

**Clinical Significance:**
- Thin muscles: More vulnerable to atrophy, visible movement
- Thick muscles: Greater contractile force, better preservation with age

### Distance Maps

**Interpretation:**
- **Units:** Millimeters
- **Zero values:** Muscle directly adjacent to bone
- **Small values (0-10 mm):** Deep muscle layer, closely associated with skeleton
- **Large values (10-50 mm):** Superficial layers, distant from bone

**Clinical Significance:**
- Deep muscles: Primary movers, more stable
- Superficial muscles: Expression muscles, movement generators

### Statistical Outputs (CSV)

**stat.csv - Muscle Group Counts**
```
ID              N_Unknown  N_Mimik  N_Platysma  N_Mastication  N_Eye  N_Tongue
Subject_001_HS  1250       45600    2300        8900           3200   1100
```

**stat_rater.csv - Rater Agreement**
```
ID              Voxels_Agreement  Voxels_Partial  Voxels_Disagreement
Subject_001_HS  42300             3200            1500
```

**stat_*_sub.csv - Regional Analysis**
```
Region  Thickness_Mean  Thickness_SD  Distance_Mean  Fat_Volume  Bone_Volume
1       8.5             2.3           15.2           450         120
2       9.1             2.1           14.8           480         135
...
```

## Scientific Background

### Facial Anatomy Overview

**Major Muscle Groups:**

1. **Mimic Muscles** (facial expression)
   - Orbicularis oculi (eye)
   - Orbicularis oris (mouth)
   - Buccinator (cheek)
   - Corrugator supercilii (eyebrow)
   - Frontalis (forehead)
   - Platysma (neck/lower face)

2. **Masticatory Muscles**
   - Masseter (elevation, closure)
   - Temporalis (elevation, closure)
   - Medial pterygoid (elevation, closure)
   - Lateral pterygoid (opening, lateral movement)
   - Mylohyoid (supporting musculature)

3. **Eye Muscles**
   - Orbicularis oculi (closure, blinking)
   - Levator palpebrae superioris (opening)
   - Mueller's muscle (sympathetic contribution)

4. **Pharyngeal & Lingual Muscles**
   - Tongue muscles (intrinsic and extrinsic)
   - Pharyngeal constrictor muscles
   - Soft palate muscles

### Muscle Organization

**Layering:**
- **Deep layer:** Attached to bone, primary movers
- **Intermediate layer:** Crossing muscle fibers, coordinative function
- **Superficial layer:** Expression muscles, attached to skin/fascia

**Fiber Organization:**
- **Parallel fibers:** Greater contractile force
- **Fusiform:** Balanced strength and motion range
- **Complex arrangements:** Coordinated multi-directional movement

### MRI Imaging Principles

**T1-Weighted Images:**
- **Muscle:** Intermediate signal (medium gray)
- **Fat:** High signal (bright white)
- **Bone:** Low signal (dark)
- **Best for:** Anatomical detail, tissue identification

**FLAIR Sequences:**
- Suppresses CSF signal
- Enhanced muscle-to-fluid contrast
- Better for facial region imaging

## Related Research & References

### Associated Projects

- **Cloud2** - 3D visualization and geometric analysis
  - https://github.com/heikostark/Cloud2
  - Used for visualizing 3D facial anatomy

- **imagexd** - Core image processing framework
  - https://stark-jena.de/research-interests/software/imagexd/
  - All processing executed through this tool

- **VHP-Classification** - Connective tissue analysis
  - https://github.com/heikostark/VHP-classification
  - Similar multi-rater consensus methodology

- **Gordon1966** - Biomechanical muscle modeling
  - https://github.com/heikostark/Gordon1966
  - Integration with FEBio for muscle simulation

### Research References

**Facial Anatomy:**
- Anatomical classification systems
- 3D reconstruction techniques
- Biomechanical properties of facial muscles

**Inter-Rater Agreement:**
- Consensus methodology
- Reliability statistics (Dice coefficient, Jaccard index)
- Agreement assessment metrics

**MRI Segmentation:**
- Anatomical accuracy in medical imaging
- Multi-rater segmentation protocols
- Quality assessment methods

### Research Website

For more information on 3D facial anatomy research:
- https://stark-jena.de/current-responsibilities/3d-face/
- Research focus and publications
- Collaborators and partnerships

## Muscle Groups Defined

### Classification System

**Class 0 - Unknown/Unclassified**
- Tissue not clearly assigned to a muscle group
- Requires further classification
- Often represents partial volume effects

**Class 1 - Mimic Muscles**
- Muscles of facial expression
- Primarily attached to skin
- Innervated by CN VII (facial nerve)
- Include: orbicularis oculi/oris, buccinator, corrugators

**Class 2 - Platysma**
- Large sheet muscle of lower face and neck
- Extends from mandible to clavicle/shoulder
- Important for facial expression and neck movement
- Often analyzed separately due to unique anatomy

**Class 3 - Masticatory Muscles**
- Muscles specialized for jaw movement and biting
- Include: masseter, temporalis, medial/lateral pterygoid
- Innervated by CN V (trigeminal nerve)
- Critical for feeding and speech

**Class 4 - Eye Muscles**
- Muscles controlling eye and eyelid movement
- Include: orbicularis oculi, levator palpebrae
- Essential for vision protection and expression
- Involved in blinking and eye closure

**Class 5 - Tongue/Pharyngeal Muscles**
- Muscles of tongue and pharynx
- Critical for speech and swallowing
- Mix of intrinsic and extrinsic muscles
- Innervated by CN XII (hypoglossal) and others

## Performance Considerations

### Computational Requirements

**Typical System Specifications:**
- **CPU:** Modern multi-core processor (8+ cores recommended)
- **RAM:** 32 GB minimum, 64+ GB recommended
- **Storage:** 500 GB - 2 TB depending on dataset size
- **GPU:** Optional (accelerated processing with CUDA support)

**Processing Times (Approximate per subject):**
- Coordinate alignment: 1-2 minutes
- Consensus generation: 1-2 minutes (per operation)
- Thickness calculation: 5-10 minutes
- Distance calculation: 5-10 minutes
- Statistical analysis: 1-5 minutes

**Total Pipeline:** ~30-60 minutes per subject (depending on volume)

### Memory Management

**Memory-Intensive Operations:**
- 3D distance transform
- Thickness measurement with large scan radii
- Multi-subject batch processing

**Optimization Tips:**
1. Process one subject at a time
2. Adjust thread count based on available RAM
3. Use disk caching for intermediate results
4. Monitor memory usage during processing

### Parallelization

**Thread Settings:**
```
thread.max := 40
// Optimal: 0.8 × number of available cores
// Example: 40 cores → thread.max := 32
```

**Batch Parallelization:**
- Run multiple subjects simultaneously on separate cores
- Use GNU Parallel or similar tools
- Monitor total system memory

## Data Quality Assurance

### Rater Agreement Analysis

**Metrics Computed:**
- **Dice Coefficient:** Overlap between raters
- **Jaccard Index:** Union-to-intersection ratio
- **Hausdorff Distance:** Maximum distance between segmentations
- **Volume agreement:** Voxel count consistency

**Interpretation:**
- **Dice ≥ 0.85:** Good agreement
- **Dice 0.70-0.85:** Acceptable agreement
- **Dice < 0.70:** Poor agreement (review needed)

### Quality Control Steps

**Before Processing:**
1. Verify file formats and naming conventions
2. Check coordinate system alignment
3. Validate landmark detection
4. Inspect individual rater segmentations visually

**During Processing:**
1. Monitor transformation accuracy
2. Verify consensus threshold appropriateness
3. Check output file completeness
4. Validate statistical results

**After Processing:**
1. Compare results to known anatomy
2. Check for outliers in statistical analysis
3. Validate thickness/distance measurements
4. Visual inspection of results

## Troubleshooting

### Common Issues

**Problem:** "File not found" errors
- **Solution:** Verify file naming follows conventions (mask_*.nii.gz)
- Check directory structure matches expected format
- Verify all required files are present

**Problem:** Coordinate alignment failures
- **Solution:** Check marker file format (.mrk.json)
- Verify landmark names match script expectations
- Validate marker file locations

**Problem:** Consensus threshold not met
- **Solution:** Ensure sufficient raters (minimum 2 for best.macro)
- Check rater segmentation quality
- Review rater agreement statistics

**Problem:** Out of memory errors
- **Solution:** Reduce thread count
- Process fewer subjects simultaneously
- Increase available system RAM
- Use 64-bit imagexd version

**Problem:** Slow processing
- **Solution:** Increase thread count (if resources available)
- Verify disk I/O not bottlenecked
- Check for competing processes
- Consider SSD for data storage

**Problem:** Inconsistent results across runs
- **Solution:** Verify identical input data
- Check for floating-point precision issues
- Validate environmental variables
- Review imagexd version consistency

## Contributing

Contributions are welcome! Areas for enhancement:

### Potential Contributions

- Additional anatomical classification schemes
- Improved inter-rater agreement algorithms
- Performance optimizations
- Extended statistical analysis methods
- Validation against manual measurements
- Documentation improvements
- Example datasets

### How to Contribute

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/improvement`)
3. Make improvements with clear documentation
4. Test thoroughly with sample data
5. Submit pull request with detailed description

## Citation

If you use this work in your research, please cite:

```bibtex
@repository{3d-face-classification,
  title={3D-FACE-Classification: Advanced Analysis and Classification 
         of Facial and Mimic Muscle Anatomy using 3D MRI Data},
  author={Stark, Heiko},
  year={2026},
  url={https://github.com/heikostark/3D-FACE-classification}
}
```

**Associated Research Topics:**
- Facial anatomy analysis
- 3D medical image segmentation
- Multi-rater consensus methods
- Muscle classification systems
- Clinical imaging applications

## License

This project is licensed under the **GNU General Public License v3.0** (GPL-3.0).

You are free to:
- ✅ Use the software for any purpose
- ✅ Study how it works
- ✅ Modify and improve it
- ✅ Share it with others

With the requirement to:
- ℹ️ Provide attribution to the original author
- ℹ️ Disclose source modifications
- ℹ️ Use the same license for derivative works

See LICENSE file for full terms and conditions.

## Author and Attribution

**Heiko Stark**  
Department of Medical Imaging and Simulation  
University of Jena

**Research Interests:**
- Medical image analysis
- 3D anatomical reconstruction
- Facial anatomy classification
- Multi-rater consensus methods
- Biomechanical analysis
- Computational geometry

**Website:** https://stark-jena.de/  
**Research Page:** https://stark-jena.de/current-responsibilities/3d-face/

## Acknowledgments

- **imagexd Development Team:** For the powerful image processing framework
- **Anatomical Expert Raters:** For careful manual segmentations
- **MRI Technicians:** For high-quality image acquisition
- **Research Collaborators:** For anatomical expertise and validation
- **Free and Open Source Community:** For foundational tools

## Support and Feedback

For questions, bug reports, or feature suggestions:

1. **GitHub Issues:** https://github.com/heikostark/3D-FACE-classification/issues
2. **Email:** Via GitHub profile
3. **Research Discussion:** https://stark-jena.de/

## Project Updates and Status

- **Last Updated:** May 2026
- **Repository Status:** Active
- **License:** GPL-3.0
- **Maintained By:** Heiko Stark
- **Last Modification:** November 2025

---

## Quick Reference

### File Naming Conventions

| Filename Pattern | Description | Origin |
|---|---|---|
| `mask_XX.nii.gz` | 2+ rater consensus | best.macro |
| `mask_X3.nii.gz` | 3+ rater consensus | best_x3.macro |
| `mask_[ID].nii.gz` | Individual rater | Manual segmentation |
| `mask_sub.nii.gz` | Anatomical divisions | Reference file |
| `*thickness.nii.gz` | Thickness maps | thickness.macro |
| `*distance.nii.gz` | Distance maps | distance.macro |
| `*.mrk.json` | Anatomical landmarks | Marker files |
| `*Segmentation-Bones.nii.gz` | Bone segmentation | Reference anatomy |

### Script Execution Order

1. `mrt_orientation.inc` - Coordinate system validation
2. `repair_transformation.macro` (if needed) - Fix coordinate systems
3. `best.macro` - Generate consensus
4. `best_x3.macro` (optional) - High-confidence consensus
5. `rater.macro` - Individual rater statistics
6. `thickness.macro` - Thickness calculation
7. `distance.macro` - Distance calculation
8. Statistical analysis scripts in any order:
   - `stat.macro`
   - `stat_*_sub.macro` family

### Output Files Generated

```
Results produced:
- mask_XX.nii.gz (consensus segmentation)
- mask_X3.nii.gz (high-confidence consensus)
- *_thickness.nii.gz (thickness maps)
- *_distance.nii.gz (distance maps)
- stat.csv (muscle group counts)
- stat_rater.csv (rater agreement)
- stat_*_sub.csv (regional statistics)
```

---

**For complete documentation and tutorials, visit:**  
https://stark-jena.de/current-responsibilities/3d-face/
