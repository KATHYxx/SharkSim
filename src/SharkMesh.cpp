#include "SharkMesh.h"

string SharkMesh::nextToken(char delimit, FILE* readFile)
{
	string numb;
	while(!feof(readFile))
	{
		if(ferror(readFile)){ printf("1Error reading FILE\n"); exit(-1); }
		char cur = fgetc(readFile);
		if(ferror(readFile)){ printf("2Error reading FILE\n"); exit(-1);}
		if(cur == delimit || cur == '\n') {break;}
		numb.append(1, cur);
	}
	//printf("%s \n", numb.c_str());
	return numb;
}

//FILE* SharkMesh::buildAOBJ(string filename)
void SharkMesh::buildAOBJ(FILE* readFile)
{
	vector<Vector3f> localVerts = vector<Vector3f>();   //in order to use OBJs as keys, need to remember their order.
	while(!feof(readFile))
	{
		//tokenize the line identifier. It's only one character and 
		//it should be on the first on the line
		char identifier = fgetc(readFile);
		if(ferror(readFile)){ printf("888888888Error reading FILE\n"); exit(-1);}
		

		//load in the vertices
		//v x y z nx ny nz boneName/weight boneName/weight ...
		if(identifier == 'v')
		{
			char cur = fgetc(readFile);  //space
			if(ferror(readFile)){ printf("0Error reading FILE\n"); exit(-1);}
			while(cur != '\n')  //per line
			{
				SharkVertex* sv = new SharkVertex();

				//location of vertex    
				Vector3f vert;
				vert.x = atof(nextToken(' ', readFile).c_str());
				vert.y = atof(nextToken(' ', readFile).c_str());
				vert.z = atof(nextToken(' ', readFile).c_str());
				sv->sLocal(vert);

				Vector3f nor; //normal
				nor.x = atof(nextToken(' ', readFile).c_str());
				nor.y = atof(nextToken(' ', readFile).c_str());
				nor.z = atof(nextToken(' ', readFile).c_str());
				sv->sNormal(nor);
				
				vert.Print();

				sv->sTransformed(Vector3f(0,0,0));
				//bone / weight repeats
				fseek(readFile, -1, SEEK_CUR);
				cur = fgetc(readFile);
				while(cur != '\n')
				{
					string boneName = nextToken('/', readFile);
					float weight = atof(nextToken(' ', readFile).c_str());
					sv->sBonePair(boneName, weight);
					fseek(readFile, -1, SEEK_CUR);
					cur = fgetc(readFile);
					//printf(" %s/%f", boneName.c_str(), weight);
				}
				localVerts.push_back(vert);
				insertVec(pair<Vector3f, SharkVertex*>(vert, sv));
			}
			printf("\n");
		}
		//faces
		//f ... vertices in mesh .....
		else if(identifier == 'f')
		{
			char cur = fgetc(readFile); //space
			if(ferror(readFile)){ printf("4Error reading FILE\n"); exit(-1);}
			Quad *curQuad = new Quad();
			curQuad->sNormal(Vector3f(0,0,0));
			
			//caution. Vertices listed in mesh may not be consistant with other quads 
			int vertex1 = atoi(nextToken(' ', readFile).c_str());
			int vertex2 = atoi(nextToken(' ', readFile).c_str());
			int vertex3 = atoi(nextToken(' ', readFile).c_str());
			int vertex4 = atoi(nextToken(' ', readFile).c_str());
			
			curQuad->sVert(0, gVertex(localVerts[vertex1-1]));
			curQuad->sVert(1, gVertex(localVerts[vertex2-1]));
			curQuad->sVert(2, gVertex(localVerts[vertex3-1]));
			curQuad->sVert(3, gVertex(localVerts[vertex4-1]));
			//curQuad->sNormal(curQuad->gNormal() + curQuad->gVert(0) + curQuad->gVert(1) + curQuad->gVert(2) + curQuad->gVert(3));
			curQuad->sNormal(curQuad->gNormal() / 4.0);

			pushFace(curQuad);	
			//neighboring quads are to be found later, after parsing is done.

			fseek(readFile, -1, SEEK_CUR);
			cur = fgetc(readFile);
			while(cur != '\n')
			{
				cur = fgetc(readFile);
				if(ferror(readFile)){ printf("5Error reading FILE\n"); exit(-1);}
				//fseek(readFile, -1, SEEK_CUR);
				//cur = fgetc(readFile);
			}
		}		
		//bones
		//b name headRestArmature tailRestArmature ... child names ...
		//read outside of this function	
		else if(identifier == 'b')
		{
			return;
		}
	}
}

/*Restores vertex transformation back to rest pose */
void SharkMesh::restPosition()
{
	map<Vector3f, SharkVertex*, compareVect3>::iterator im;
	for(im = vertices.begin(); im != vertices.end(); im++ )
	{
		im->second->transformed = im->second->local;
	}
}

//TODO delete Heap properly.
void SharkMesh::deleteHeap()
{
	map<Vector3f, SharkVertex*, compareVect3>::iterator im;
	for(im = vertices.begin(); im != vertices.end(); im++ )
	{
		//delete each element
		delete im->second;
	}
}
