int main(){

	int n = readInt();
	int a[103];
	
	int i;
	for(i=0;i<n;i++){
		a[i]=readInt();
	}

	int j;
	int temp;
	for(i=1;i<n;i++){
		j=i-1;
		while(j>=0 && a[j]>a[j+1]){
			temp=a[j];
			a[j]=a[j+1];
			a[j+1]=temp;
			j--;
		}
	}

	for(i=0;i<n;i++){
		print(a[i], ' ');
	}
	print('\n');

	return 0;
}