# echo -e "Diabetes"
# ./kmeans , Datasets/diabetes diabetes.arff 2 y > Datasets/diabetes/out

# echo -e "\nGlass"
# ./kmeans , Datasets/glass glass.arff 7 y > Datasets/glass/out

# echo -e "\nIris"
# ./kmeans , Datasets/iris iris.arff 3 y > Datasets/iris/out

# echo -e "\nIris2D"
# ./kmeans , Datasets/iris2d iris.2D.arff 3 y > Datasets/iris2d/out

# echo -e "\nIonosphere"
# ./kmeans , Datasets/ionosphere ionosphere.arff 2 y > Datasets/ionosphere/out

# echo -e "\nSegment-Challenge"
# ./kmeans , Datasets/segment-challenge segment-challenge.arff 7 y > Datasets/segment-challenge/out

# echo -e "\nSegment-Test"
# ./kmeans , Datasets/segment-test segment-test.arff 7 y > Datasets/segment-test/out

# echo -e "\nUnbalanced"
# ./kmeans , Datasets/unbalanced unbalanced.arff 2 y > Datasets/unbalanced/out

echo -e "\n3DRoadNetwork"
./kmeans , Datasets/3DRoadNetwork 3D_spatial_network.txt 3 4 n > Datasets/3DRoadNetwork/out
