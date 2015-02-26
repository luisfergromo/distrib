double dist(struct AVION a,struct AVION b)
{
	double dist_x,dist_y,dist_z,dist;
	dist_x=a.pos_x-b.pos_x;
	dist_y=a.pos_y-b.pos_y;
	dist_z=a.pos_z-b.pos_z;
	dist=sqrt(dist_x*dist_x+dist_y*dist_y+dist_z*dist_z);
	return(dist);
}

double distmin(struct AVION a,struct AVION b)
{
	double speed;
	double distmin;
	if(a.currspeed>b.currspeed)
		speed=a.currspeed/3.6;
	else
		speed=b.currspeed/3.6;
	distmin=MINDIST+speed*speed*1.6;
	return(distmin);
}

void inicializa_aviones(struct AVION *x)
{
	int i;
	for(i=0;i<MAX;i++)
	{
		x[i].pos_x=1000*(5000.0 - (double) (rand()%10000));
		x[i].pos_y=1000*(5000.0 - (double) (rand()%10000));
		x[i].pos_z=(double) (rand()%13000);
		x[i].currspeed=MINSPEED+x[i].pos_z/21;
		x[i].dir=(double) (rand()%360);
	}
}

// Se actualiza cada 1/1000 segundos
void actualiza_avion(struct AVION *a)
{
	if(a->pos_z<13000.0)
		a->pos_z+=0.001;
	a->currspeed=MINSPEED+a->pos_z/21;
	a->pos_x+=0.001*(a->currspeed/3.6)*SIN(a->dir);
	a->pos_y+=0.001*(a->currspeed/3.6)*COS(a->dir);
}