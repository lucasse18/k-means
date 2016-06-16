args <- commandArgs(trailingOnly = TRUE)
yy <- read.csv(args[3], header=FALSE)
yy_acc <- read.csv(args[4], header=FALSE)
mclust::adjustedRandIndex(yy, yy_acc)
