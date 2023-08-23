int=0
content=$(./test.sh)
ret=$?
while ((ret == 0))
do
	int=$((int+1))
	content=$(./test.sh)
	ret=$?
done
echo "$content" > error.txt
echo "$int times success" >> error.txt
cat error.txt
	
