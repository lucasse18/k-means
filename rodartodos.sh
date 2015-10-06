echo -e "Diabetes"
./kmeans , Datasets/diabetes/cini Datasets/diabetes/cfim Datasets/diabetes/melhor 2 y < Datasets/diabetes/diabetes.arff > Datasets/diabetes/out

echo -e "\nGlass"
./kmeans , Datasets/glass/cini Datasets/glass/cfim Datasets/glass/melhor 7 y < Datasets/glass/glass.arff > Datasets/glass/out

echo -e "\nIris"
./kmeans , Datasets/iris/cini Datasets/iris/cfim Datasets/iris/melhor 3 y < Datasets/iris/iris.arff > Datasets/iris/out

echo -e "\nIris2D"
./kmeans , Datasets/iris2d/cini Datasets/iris2d/cfim Datasets/iris2d/melhor 3 y < Datasets/iris2d/iris.2D.arff > Datasets/iris2d/out

echo -e "\nIonosphere"
./kmeans , Datasets/ionosphere/cini Datasets/ionosphere/cfim Datasets/ionosphere/melhor 2 y < Datasets/ionosphere/ionosphere.arff > Datasets/ionosphere/out

echo -e "\nSegment-Challenge"
./kmeans , Datasets/segment-challenge/cini Datasets/segment-challenge/cfim Datasets/segment-challenge/melhor 7 y < Datasets/segment-challenge/segment-challenge.arff > Datasets/segment-challenge/out

echo -e "\nSegment-Test"
./kmeans , Datasets/segment-test/cini Datasets/segment-test/cfim Datasets/segment-test/melhor 7 y < Datasets/segment-test/segment-test.arff > Datasets/segment-test/out

echo -e "\nUnbalanced"
./kmeans , Datasets/unbalanced/cini Datasets/unbalanced/cfim Datasets/unbalanced/melhor 2 y < Datasets/unbalanced/unbalanced.arff > Datasets/unbalanced/out

echo -e "\n3DRoadNetwork"
./kmeans , Datasets/3DRoadNetwork/cini Datasets/3DRoadNetwork/cfim Datasets/3DRoadNetwork/melhor 2 n < Datasets/3DRoadNetwork/3D_spatial_network.txt > Datasets/3DRoadNetwork/out
