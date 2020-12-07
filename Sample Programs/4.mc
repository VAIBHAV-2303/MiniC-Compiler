int main(){

	int limit=100000000;
	int count=0;

	int x;
	int y;
	int z;
	
	int m=2;
	int n;
	int k;
	while(m*m<=limit){
		for(n=1;n<m;n++){
			
			x=m*m - n*n;
			y=2*m*n;
			z=m*m + n*n;
				
			k=1;
			while(k*z<=limit){
				k++;
				count++;
			}
		}
		m++;
	}
	print(count, '\n');

	return 0;
}