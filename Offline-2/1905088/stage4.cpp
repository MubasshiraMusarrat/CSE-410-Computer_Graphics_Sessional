#include "stage3.hpp"
#include "bitmap_image.hpp"

using namespace std;

int screenHeight;
int screenWidth;

vector<vector3D> colorBuffer;
extern vector<vector3D> vertices;

double **zBuffer;
double zMax = 1;
double zFrontLimit = -1;

double dx, dy;
double topY, bottomY, leftX, rightX;
double leftLimit = -1;
double rightLimit = 1;
double topLimit = 1;
double bottomLimit = -1;

bitmap_image image;

static unsigned long int g_seed = 1;  

inline int random() { 
g_seed = (214013 * g_seed + 2531011);  
return (g_seed >> 16) & 0x7FFF; 
} 

void initZBuffer(){
    dx = (rightLimit - leftLimit)/screenWidth;
    dy = (topLimit - bottomLimit)/screenHeight;

    topY = topLimit - dy/2;
    bottomY = bottomLimit + dy/2;

    leftX = leftLimit + dx/2;
    rightX = rightLimit - dx/2;

    zBuffer = new double*[screenHeight];
    for(int i=0; i<screenHeight; i++){
        zBuffer[i] = new double[screenWidth];
        for(int j=0; j<screenWidth; j++){
            zBuffer[i][j] = zMax;
        }
    }
}

double calcZ(vector3D v1, vector3D v2, vector3D v3, double x, double y){
    vector3D normal = ((v2-v1).cross(v3-v1)).normalize();
    double A = normal.getVal(0);
    double B = normal.getVal(1);
    double C = normal.getVal(2);
    double D = -(A*v1.getVal(0) + B*v1.getVal(1) + C*v1.getVal(2));
    double z = -(A*x + B*y + D)/C;
    return z;
}

void updateZBuffer(){
for(int i=0; i<vertices.size(); i+=3){
    double maxY = max(vertices[i].getVal(1), max(vertices[i+1].getVal(1), vertices[i+2].getVal(1)));
    double top_scanline = min(maxY, topY);

    double minY = min(vertices[i].getVal(1), min(vertices[i+1].getVal(1), vertices[i+2].getVal(1)));
    double bottom_scanline = max(minY, bottomY);

    int top_scanline_index = (topY - top_scanline)/dy;
    int bottom_scanline_index = (topY - bottom_scanline)/dy;

    for(int row_no = top_scanline_index; row_no <= bottom_scanline_index; row_no++){
        double y = topY - row_no*dy;

        double minX = rightX;
        double maxX = leftX;

        for(int j=0; j<3; j++){
            double x1 = vertices[i+j].getVal(0);
            double y1 = vertices[i+j].getVal(1);

            double x2 = vertices[i+(j+1)%3].getVal(0);
            double y2 = vertices[i+(j+1)%3].getVal(1);

            if((y1 > y && y2 >y) || (y1 < y && y2 < y)){
                continue;
            }

            double m = (y2-y1)/(x2-x1);
            double x = m == 0 ? x2 : (x1 + (y-y1)/m);
            maxX = max(maxX, x);
            minX = min(minX, x);
        }

        double leftIntersection = max(minX, leftX);
        double rightIntersection = min(maxX, rightX);

        int leftIntersectionIndex = (leftIntersection - leftX)/dx + 1;
        int rightIntersectionIndex = (rightIntersection - leftX)/dx;

        for (int col_no= leftIntersectionIndex; col_no <= rightIntersectionIndex; col_no++){
            double x = leftX + col_no * dx;

            if(x < leftLimit || x > rightLimit || y < bottomLimit || y > topLimit){
                continue;
            }

            double z = calcZ(vertices[i], vertices[i+1], vertices[i+2], x, y);

            if(z < zFrontLimit || z > zMax){
                continue;
            }

            if(z < zBuffer[row_no][col_no]){
                zBuffer[row_no][col_no] = z;
                image.set_pixel(col_no, row_no, colorBuffer[i/3].getVal(2), colorBuffer[i/3].getVal(1), colorBuffer[i/3].getVal(0));
            }
        }
    }
}
}

int main(){
    modelingTransformation();
    viewTransformation();
    projectionTransformation();

    ifstream configFile("config.txt");
    ofstream output4File("z_buffer.txt");
    configFile >> screenWidth >> screenHeight;
    // cout << screenWidth << " " << screenHeight << endl;

    for(int i=0; i<vertices.size(); i+=3){
        vector3D c(random() % 256, random() % 256, random() % 256);
        colorBuffer.push_back(c);
    }
    
    initZBuffer();

    image.setwidth_height(screenWidth, screenHeight);
    image.set_all_channels(0, 0, 0);

    updateZBuffer();
    
    for(int i=0; i<screenHeight; i++){
        for(int j=0; j<screenWidth; j++){
            if(zBuffer[i][j] < zMax){
                output4File << fixed << setprecision(6) << zBuffer[i][j] << "\t";
            }
        }
        output4File << endl;
    }
    
    image.save_image("out.bmp");

    image.clear();
    for(int i=0; i<screenHeight; i++){
        delete[] zBuffer[i];
    }
    delete[] zBuffer;
    vertices.clear();
    colorBuffer.clear();

    configFile.close();
    output4File.close();

    return 0;
}