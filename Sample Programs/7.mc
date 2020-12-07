int power(int x, int y){
	int ans=1;
	while(y>0){
		if(y%2==1){
			ans=ans*x;
		}
		x=x*x;
		y=y/2;
	}	
	return ans;
}

int getDig(int n, int d){
	n=n/power(10, d-1);
	return n%10;
}

int main(){

	int n = readInt();
	int a[103];
	int temp[10][103];
	int sz[10];
	
	int i;
	for(i=0;i<n;i++){
		a[i]=readInt();
	}
		
	int d;
	int t;
	int j;
	int k;
	for(i=1;i<=9;i++){
		for(j=0;j<10;j++){
			sz[j]=0;
		}

		for(j=0;j<n;j++){
			d=getDig(a[j], i);
			temp[d][sz[d]]=a[j];
			sz[d]++;
		}

		t=0;
		for(j=0;j<10;j++){
			for(k=0;k<sz[j];k++){
				a[t++]=temp[j][k];
			}
		}
	}

	for(i=0;i<n;i++){
		print(a[i], ' ');
	}
	print('\n');

	return 0;
}