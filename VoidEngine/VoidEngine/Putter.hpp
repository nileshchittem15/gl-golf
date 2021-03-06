#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <GL/freeglut.h>
#include "GolfBall.hpp"

class Putter
{
public:
                            Putter(GolfBall* golfBall);
    std::vector<glm::vec3>	getVertices() const;
    std::vector<glm::vec3>  getNormals() const;
    std::vector<GLuint>     getIndices() const;
    glm::vec3	            getPosition() const;
    glm::mat4               getModelMatrix() const;
	int						getStroke();
    void					setPosition(glm::vec3 position);
    virtual void			handleKeyboard(char input, float deltaTime);

	

private:
    void                    pushIndices(int sectors, int r, int s);

private:

    std::vector<glm::vec3>	mVertices;
    std::vector<glm::vec3>  mNormals;
    std::vector<GLuint>     mIndices;
    float					mAngle;
    float                   mPower;
	int						mStroke;
	bool					mlogic;
    glm::vec3				mPosition;
    GolfBall*				mGolfBall;

};