int a[103];
int L[103];
int R[103];

int merge(int l, int r){
	
	int i;
	int j;
	for(i=l;i<=(l+r)/2;i++){
		L[i-l]=a[i];
	}
	for(j=((l+r)/2)+1;j<=r;j++){
		R[j-((l+r)/2)-1]=a[j];
	}

	int ls = (l+r)/2 - l + 1;
	int rs = r - ((l+r)/2);
	
	i=0; 
	j=0; 
	int t=l;
	while(i<ls && j<rs){
		if(L[i]<R[j]){
			a[t++]=L[i++];
		}
		else{
			a[t++]=R[j++];
		}
	}

	while(i<ls){
		a[t++]=L[i++];
	}
	while(j<rs){
		a[t++]=R[j++];
	}

	return 0;
}

int mergesort(int l, int r){
	if(r>l){
		mergesort(l, (l+r)/2);
		mergesort(((l+r)/2)+1, r);
		merge(l, r);
	}
	return 0;
}

int main(){

	int n = readInt();

	int i;
	for(i=0;i<n;i++){
		a[i] = readInt();
	}
	
	mergesort(0, n-1);

	for(i=0;i<n;i++){
		print(a[i], ' ');
	}
	print('\n');

	return 0;
}