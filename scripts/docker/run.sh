#!/bin/bash

Rscript reports/01-generate-instances.R
Rscript reports/02-build-performance-data.R
Rscript reports/03-best-solver.R
Rscript reports/04-build-features-data.R
Rscript reports/05-build-models.R
