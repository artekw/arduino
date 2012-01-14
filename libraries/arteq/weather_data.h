typedef struct {
	int nodeid;
	int light;
	float humi;
	float temp;
	float pressure;
	byte lobat		:1;
	int battvol;
	int solvol;
	byte solar		:1;
	byte bat		:1;
} Payload;
Payload measure;
