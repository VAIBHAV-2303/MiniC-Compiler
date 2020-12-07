int a[103];
int temp[103];

int fn(int pos, int n){
	
	int i;

	if(pos>n){
		for(i=1;i<=n;i++){
			print(temp[i], ' ');
		}
		print('\n');
	}
	else{
		for(i=1;i<=n;i++){
			if(a[i]==1){
				a[i]=0;
				temp[pos]=i;
				fn(pos+1, n);
				a[i]=1;
			}
		}
	}
	return 0;
}	

int main(){

	int n = readInt();
	int i;
	for(i=1;i<=n;i++){
		a[i]=1;
	}
	fn(1, n);

	return 0;
}