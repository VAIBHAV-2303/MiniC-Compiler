int main(){

	int a[103][103];
	int b[103][103];
	int c[103][103];
	
	int p=readInt();
	int q=readInt();
	int r=readInt();

	int i;
	int j;
	for(i=0;i<p;i++){
		for(j=0;j<q;j++){
			a[i][j]=readInt();
		}
	}
	for(i=0;i<q;i++){
		for(j=0;j<r;j++){
			b[i][j]=readInt();
		}
	}

	int k;
	for(i=0;i<p;i++){
		for(j=0;j<r;j++){
			c[i][j]=0;
			for(k=0;k<q;k++){
				c[i][j]=c[i][j]+a[i][k]*b[k][j];
			}
		}
	}

	for(i=0;i<p;i++){
		for(j=0;j<r;j++){
			print(c[i][j], ' ');
		}
		print('\n');
	}

	return 0;
}