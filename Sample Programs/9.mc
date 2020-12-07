int main(){

	int a[103][103];
	int b[103][103];
	int c[103][103];
	int n = readInt();
	int m = readInt();

	int i;
	int j;
	for(i=0;i<n;i++){
		for(j=0;j<m;j++){
			a[i][j]=readInt();
		}
	}
	for(i=0;i<n;i++){
		for(j=0;j<m;j++){
			b[i][j]=readInt();
		}
	}

	# Adding the two matrices
	for(i=0;i<n;i++){
		for(j=0;j<m;j++){
			c[i][j]=a[i][j]+b[i][j];
		}
	}

	# Printing the final answer
	for(i=0;i<n;i++){
		for(j=0;j<m;j++){
			print(c[i][j], ' ');
		}
		print('\n');
	}

	return 0;
}