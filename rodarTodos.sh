echo -e "Diabetes"
./kmeans , Datasets/diabetes 2 y < Datasets/diabetes/diabetes.arff > Datasets/diabetes/out

echo -e "\nGlass"
./kmeans , Datasets/glass 7 y < Datasets/glass/glass.arff > Datasets/glass/out

echo -e "\nIris"
./kmeans , Datasets/iris 3 y < Datasets/iris/iris.arff > Datasets/iris/out

echo -e "\nIris2D"
./kmeans , Datasets/iris2d 3 y < Datasets/iris2d/iris.2D.arff > Datasets/iris2d/out

echo -e "\nIonosphere"
./kmeans , Datasets/ionosphere 2 y < Datasets/ionosphere/ionosphere.arff > Datasets/ionosphere/out

echo -e "\nSegment-Challenge"
./kmeans , Datasets/segment-challenge 7 y < Datasets/segment-challenge/segment-challenge.arff > Datasets/segment-challenge/out

echo -e "\nSegment-Test"
./kmeans , Datasets/segment-test 7 y < Datasets/segment-test/segment-test.arff > Datasets/segment-test/out

echo -e "\nUnbalanced"
./kmeans , Datasets/unbalanced 2 y < Datasets/unbalanced/unbalanced.arff > Datasets/unbalanced/out

echo -e "\n3DRoadNetwork"
./kmeans , Datasets/3DRoadNetwork 5 n < Datasets/3DRoadNetwork/3D_spatial_network.txt > Datasets/3DRoadNetwork/out

