#define _USE_MATH_DEFINES
#include <cmath>
#include "Level.hpp"

Level* Level::sInstance = nullptr;

Level::Level(std::vector<Tile> tiles, LevelObject tee, LevelObject cup, int par) : mGolfBall(glm::vec3(0.0f, 0.0f, 0.0f), 0.05f, 50, 50), mPutter(&mGolfBall)
{
    sInstance = this;
    mCurrentPlayer = 0;

    // Store level information
    mTiles = tiles;
    mTee = tee;
    mCup = cup;
	mpar = par;

    mGolfBall.setPosition(mTee.position);
    mGolfBall.setTileID(mTee.tileID);

    // Process level information
    processTiles();
    processTee();
    processCup();
}

Level::~Level()
{

}

Level* Level::getInstance()
{
    return sInstance;
}

std::vector<Tile> Level::getTiles() const
{
    return mTiles;
}

std::vector<glm::vec3> Level::getTilesVertices() const
{                       
    return mTilesVertices;
}                       
                        
std::vector<glm::vec3> Level::getTilesNormals() const
{                       
    return mTilesNormals;
}                       
                        
std::vector<GLuint> Level::getTilesIndices() const
{
    return mTilesIndices;
}

std::vector<Wall> Level::getWalls() const
{
    return mWalls;
}

std::vector<glm::vec3> Level::getWallsVertices() const
{
    return mWallsVertices;
}

std::vector<glm::vec3> Level::getWallsNormals() const
{
    return mWallsNormals;
}

std::vector<GLuint> Level::getWallsIndices() const
{
    return mWallsIndices;
}

Level::LevelObject Level::getTee() const
{
    return mTee;
}

std::vector<glm::vec3> Level::getTeeVertices() const
{
    return mTeeVertices;
}

std::vector<glm::vec3> Level::getTeeNormals() const
{
    return mTeeNormals;
}

std::vector<GLuint> Level::getTeeIndices() const
{
    return mTeeIndices;
}

Level::LevelObject Level::getCup() const
{
    return mCup;
}

std::vector<glm::vec3> Level::getCupVertices() const
{
    return mCupVertices;
}

std::vector<glm::vec3> Level::getCupNormals() const
{
    return mCupNormals;
}

std::vector<GLuint> Level::getCupIndices() const
{
    return mCupIndices;
}
int Level::getParNum()
{
	return mpar;
}

GolfBall* Level::getGolfBall()
{
    return &mGolfBall;
}

Putter* Level::getPutter()
{
    return &mPutter;
}

bool Level::getComplete() const
{
    return (mComplete[0] && mComplete[1]);
}

unsigned Level::getCurrentPlayer() const
{
    return mCurrentPlayer + 1;
}

void Level::setTiles(std::vector<Tile> tiles)
{
    mTiles = tiles;
}

void Level::setTee(LevelObject tee)
{
    mTee = tee;
}

void Level::setCup(LevelObject cup)
{
    mCup = cup;
}

void Level::setComplete(bool complete)
{
    mComplete[mCurrentPlayer] = complete;
    if (mCurrentPlayer == 0)
        changePlayer();
}

void Level::changePlayer()
{
    if (mCurrentPlayer == 0)
    {
        mCurrentPlayer = 1;
    }
    else
    {
        mCurrentPlayer = 0;
    }
}

void Utility::processVerts(std::vector<glm::vec3> &points, std::vector<glm::vec3> &verts, std::vector<GLuint> &indices)
{
    // Add first triangle of the polygon
    // Add the first point and store it for reuse
    GLuint first = checkIndice(verts, points[0]);
    indices.push_back(first);

    // Add the middle point of the triangle
    indices.push_back(checkIndice(verts, points[1]));

    // Add the last point of the triangle and store it for reuse
    GLuint last = checkIndice(verts, points[2]);
    indices.push_back(last);

    // Iterate through the remaining points in the polygon and generate triangles
    for (unsigned j = 3; j < points.size(); j++)
    {
        // Add first and old last indices
        indices.push_back(first);
        indices.push_back(last);

        // Get the next indice, add it, and store it as the new last
        GLuint indice = checkIndice(verts, points[j]);
        indices.push_back(indice);
        last = indice;
    }
}

void Level::processTiles()
{ 
    // Iterate through all tiles and vertices in structure.
    for (unsigned i = 0; i < mTiles.size(); i++)
    {
        // Get and store the tile's points
        std::vector<glm::vec3> points = mTiles[i].getVertices();
        
        Utility::processVerts(points, mTilesVertices, mTilesIndices);

        // Get Neighbors
        std::vector<int> neighbors = mTiles[i].getNeighbors();
        // Search through neighbors
        for (unsigned j = 0; j < neighbors.size(); j++)
        {
            // If there is no neighbor create a wall
            if (neighbors[j] == 0)
            {
                std::vector<glm::vec3> wall;
                // Get ground points
                wall.push_back(points[j]);
                wall.push_back(points[j] + glm::vec3(0.0, 0.15, 0.0));
                if (j + 1 == neighbors.size())
                {
                    wall.push_back(points[0] + glm::vec3(0.0, 0.15, 0.0));
                    wall.push_back(points[0]);
                }
                else
                {
                    wall.push_back(points[j + 1] + glm::vec3(0.0, 0.15, 0.0));
                    wall.push_back(points[j + 1]);
                }
                Utility::processVerts(wall, mWallsVertices, mWallsIndices);

                // Generate and store the normals for the wall
                glm::vec3 normal = Utility::calculateNormal(wall);
                for (unsigned k = 0; k < wall.size(); k++)
                {
                    mWallsNormals.push_back(normal);
                }

                // Create and save the wall object
                Wall* wallObj = new Wall(wall);
                wallObj->setNormal(normal);
                wallObj->setTileID(mTiles[i].getID());
                mTiles[i].addWall(wallObj);
                mWalls.push_back(*wallObj);
            }
        }

        // Generate and store face normals
        glm::vec3 normal = Utility::calculateNormal(points);
        for (unsigned j = 0; j < points.size(); j++)
        {
            mTilesNormals.push_back(normal);
        }

        mTiles[i].setNormal(normal);
    }
}

glm::vec3 Utility::calculateNormal(const std::vector<glm::vec3> &points)
{
    glm::vec3 p1 = points[0];
    glm::vec3 p2 = points[1];
    glm::vec3 p3 = points[2];

    glm::vec3 v = p2 - p1;
    glm::vec3 w = p3 - p1;

    float nx = (v.y * w.z) - (v.z * w.y);
    float ny = (v.z * w.x) - (v.x * w.z);
    float nz = (v.x * w.y) - (v.y * w.x);

    glm::vec3 normals(nx, ny, nz);
    return glm::normalize(normals);
}

// Checks if indice already exists in indice list, if so returns that point, otherwise returns new one
GLuint Utility::checkIndice(std::vector<glm::vec3> &verts, glm::vec3 point)
{
    // Check if point already exists in verts, if so return its position
    //for (unsigned i = 0; i < verts.size(); i++)
    //{
    //    if (verts[i] == point)
    //    {
    //        return i;
    //    }
    //}
    // If point doesn't exist in verts, add it and return its position
    verts.push_back(point);
    return verts.size() - 1;
}

void Level::processTee()
{
    // Calculate Points
    std::vector<glm::vec3> points;
    points.push_back(mTee.position - glm::vec3(-0.1, -0.001, -0.1));
    points.push_back(mTee.position - glm::vec3( 0.1, -0.001, -0.1));
    points.push_back(mTee.position - glm::vec3( 0.1, -0.001,  0.1));
    points.push_back(mTee.position - glm::vec3(-0.1, -0.001,  0.1));

    // Process Vertices
    Utility::processVerts(points, mTeeVertices, mTeeIndices);

    // Calculate Normal
    glm::vec3 normal = Utility::calculateNormal(points);
    mTeeNormals.push_back(normal);
    mTeeNormals.push_back(normal);
    mTeeNormals.push_back(normal);
    mTeeNormals.push_back(normal);
}

void Level::processCup()
{
    std::vector<glm::vec3> points;
    points.push_back(mCup.position + glm::vec3(0.0, 0.001, 0.0));
    // Create Vertices
    const int roundness = 20;
    const double radius = 0.1;
    for (int i = 0; i < roundness; i++)
    {
        double x = sin((2 * M_PI / roundness) * i) * radius;
        double z = cos((2 * M_PI / roundness) * i) * radius;
        glm::vec3 point(x + mCup.position.x, mCup.position.y + 0.001, z + mCup.position.z);
        points.push_back(point);
    }
    
    double x = sin(0) * 0.1;
    double z = cos(0) * 0.1;
    glm::vec3 point(x + mCup.position.x, mCup.position.y + 0.001, z + mCup.position.z);
    points.push_back(point);
    
    // Process Vertices
    Utility::processVerts(points, mCupVertices, mCupIndices);

    // Calculate Normal
    glm::vec3 normal = Utility::calculateNormal(points);
    for (unsigned i = 0; i < points.size(); i++)
    {
        mCupNormals.push_back(normal);
    }
}