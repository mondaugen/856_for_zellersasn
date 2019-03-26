[ -z $BOARD_VERSION ] && echo "Specify BOARD_VERSION" && exit 1
# try to get tag
version=$(git tag --points-at HEAD)
if [ -z "$version" ]
then
    # if tag empty, get commit hash
    version=$(git log -n 1 --pretty=format:%H)
fi
echo "#define ZELLERSASN_VERSION \"${version}_${BOARD_VERSION}\""
# also store current year
year=$(date +%Y)
echo "#define ZELLERSASN_VERSION_YEAR \"$year\""
