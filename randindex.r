args <- commandArgs(trailingOnly = TRUE)
mclust::adjustedRandIndex(as.vector(args[2]), as.vector(args[3]))
