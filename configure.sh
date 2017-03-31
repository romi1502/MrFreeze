# Download Eigen v3.3.3
eigen_archive=3.3.3.tar.gz
eigen_out_dir=Eigen

curl -O https://bitbucket.org/eigen/eigen/get/$eigen_archive
mkdir -p $eigen_out_dir
tar xf $eigen_archive -C $eigen_out_dir --strip-components 1
rm $eigen_archive
