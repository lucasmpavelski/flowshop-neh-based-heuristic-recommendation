library(flowshopNehBasedHeuristicRecommendation)
library(tidyverse)

generate_test_instances()
update_instances_tars()

load_instance_data <- function(file_path) {
  info <- instance_data_from_filename(last(str_split(file_path, '/')[[1]]))
  lines <- readLines(file_path)
  no_jobs <- as.integer(lines[1])
  no_machines <- as.integer(lines[2])
  seed <- as.integer(lines[3])
  pts <- lines[seq(6, length(lines), 3)] %>%
    str_split(' ') %>%
    map(as.integer) %>%
    unlist() %>%
    matrix(nrow = no_machines, ncol = no_jobs) %>%
    t()
}

all_instances <- generated_instances_df()

View(all_instances)