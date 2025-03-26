#include <iostream>
//#include "glad/glad.h"
#include "GL/glew.h"
#include "GL/glut.h"

//#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <fstream>
#include <random>

enum KEY {
  ESC = 27,
  COMMA = 44,
  PERIOD = 46,
  SLASH = 47,
  ONE = 49,
  TWO = 50,
  THREE = 51,
  A = 97,
  B = 98,
  C = 99,
  D = 100,
  E = 101,
  F = 102,
  G = 103,
  H = 104,
  I = 105,
  J = 106,
  K = 107,
  L = 108,
  M = 109,
  N = 110,
  O = 111,
  P = 112,
  Q = 113,
  R = 114,
  S = 115,
  T = 116,
  U = 117,
  V = 118,
  W = 119,
  X = 120,
  Y = 121,
  Z = 122
};

enum Projection_type {
  perspective, orthographic
};

struct Transformation {
  glm::vec3 position;
  glm::vec3 rotation;
  float scale;
};

struct Camara {
  glm::vec3 position;
  glm::vec3 target;
  glm::vec3 up;
  Projection_type projectionType;

  void print() {
    std::cout << "position x: " << position.x << "\n";
    std::cout << "position y: " << position.y << "\n";
    std::cout << "position z: " << position.z << "\n";

    std::cout << "target x: " << target.x << "\n";
    std::cout << "target y: " << target.y << "\n";
    std::cout << "target z: " << target.z << "\n";

    std::cout << "up x: " << up.x << "\n";
    std::cout << "up y: " << up.y << "\n";
    std::cout << "up z: " << up.z << "\n";

    std::cout << "projection: " << (projectionType == perspective ? "perspective" : "orthographic") << "\n";
  }
};

struct Face {
  std::vector<int> vertices_indices;
  uint colour_index;
  uint normal_index;
};

GLenum glCheckError_(const char *file, int line) {
  GLenum errorCode;
  while ((errorCode = glGetError()) != GL_NO_ERROR) {
    std::string error;
    switch (errorCode) {
      case GL_INVALID_ENUM:
        error = "INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        error = "INVALID_VALUE";
        break;
      case GL_INVALID_OPERATION:
        error = "INVALID_OPERATION";
        break;
      case GL_STACK_OVERFLOW:
        error = "STACK_OVERFLOW";
        break;
      case GL_STACK_UNDERFLOW:
        error = "STACK_UNDERFLOW";
        break;
      case GL_OUT_OF_MEMORY:
        error = "OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        error = "INVALID_FRAMEBUFFER_OPERATION";
        break;
    }
    std::cout << error << " | " << file << " (" << line << ")" << std::endl;
  }
  return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

class RenderObject {
public:
  Transformation transformation{{0, 0, 0}, {0, 0, 0}, 1};

  bool wireframe_mode = false;
  bool draw = true;

  RenderObject() = default;
  ~RenderObject() = default;

  void generate(std::string file = {}) {
    if (file.empty()) {
      define();
    } else {
      load(file);
    }

    triangulate();

    createVertexBufferObjects(sizeOfVertices(), getVertices());
    createVertexArrayObject();
    createElementBufferObjects(sizeOfIndices(), getIndices());
  };

  void print() {
    std::cout << "\n\nscale: " << transformation.scale << "\n";

    std::cout << "position x: " << transformation.position.x << "\n";
    std::cout << "position y: " << transformation.position.y << "\n";
    std::cout << "position z: " << transformation.position.z << "\n";

    std::cout << "rotation x: " << transformation.rotation.x << "\n";
    std::cout << "rotation y: " << transformation.rotation.y << "\n";
    std::cout << "rotation z: " << transformation.rotation.z << "\n";

    std::cout << std::boolalpha << "draw: " << draw << '\n';
    std::cout << std::boolalpha << "wireframe: " << draw << '\n';
  }

  unsigned long numberOfVertices() { return _vertices_internal.size(); };

  unsigned long numberOfIndices() { return _indices_internal.size(); };

  int sizeOfVertices() { return sizeof(float) * numberOfVertices(); };

  int sizeOfIndices() { return sizeof(float) * numberOfIndices(); };

  float *getVertices() { return _vertices_internal.data(); };

  unsigned int *getIndices() { return _indices_internal.data(); };

  unsigned int getVertexBufferObjects() { return _vertexBufferObjects; };

  unsigned int getVertexArrayObject() { return _vertexArrayObject; };

  unsigned int getElementBufferObjets() { return _ElementBufferObjects; };

protected:
  std::vector<Face> _faces;
  std::vector<float> _vertices;
  std::vector<float> _colour;
  std::vector<float> _normals;

  virtual void define() {
    _vertices = {
            0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,

            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
    };

    _colour = {
            0.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 0.0, 1.0,
            0.5, 0.5, 0.5,
            1.0, 1.0, 0.0
    };

    _faces = {
            {{1, 5, 3, 7}, 0}, // bottom
            {{4, 3, 8, 7}, 1}, // top
            {{8, 7, 6, 5}, 2}, //left
            {{6, 2, 8, 4}, 3}, // right
            {{2, 1, 4, 3}, 4}, // front
            {{6, 5, 2, 1}, 5} // back
    };
  }

private:
  std::vector<float> _vertices_internal;
  std::vector<uint> _indices_internal;

  unsigned int _vertexBufferObjects{};
  unsigned int _vertexArrayObject{};
  unsigned int _ElementBufferObjects{};

  void createVertexBufferObjects(int size, float *vertices) {
    glGenBuffers(1, &_vertexBufferObjects);

    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferObjects);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
  };

  void createVertexArrayObject() {
    glGenVertexArrays(1, &_vertexArrayObject);

    glBindVertexArray(_vertexArrayObject);

    // position attribute
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          9 * sizeof(float),
                          (void *) 0);
    glEnableVertexAttribArray(0);
    // colour attribute
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          9 * sizeof(float),
                          (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // normal attribute
    glVertexAttribPointer(2,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          9 * sizeof(float),
                          (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(2);
  };

  void createElementBufferObjects(int size, unsigned int *indices) {
    glGenBuffers(1, &_ElementBufferObjects);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ElementBufferObjects);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
  };

  void triangulate() {
    std::vector<int> indices_index;
    int of = 0;
    for (Face face: _faces) {
      std::vector<float> vertex;
      std::vector<float> colour;

      std::vector<float> normal = {1.0, 1.0, 1.0};

      colour.push_back(_colour[face.colour_index * 3]);
      colour.push_back(_colour[face.colour_index * 3 + 1]);
      colour.push_back(_colour[face.colour_index * 3 + 2]);

      int j = 0;
      for (int index: face.vertices_indices) {
        int offset = index * 3;
        vertex.push_back(_vertices[offset]);
        vertex.push_back(_vertices[offset + 1]);
        vertex.push_back(_vertices[offset + 2]);

        _vertices_internal.insert(_vertices_internal.end(),
                                  vertex.begin(),
                                  vertex.end());
        _vertices_internal.insert(_vertices_internal.end(),
                                  colour.begin(),
                                  colour.end());
        _vertices_internal.insert(_vertices_internal.end(),
                                  normal.begin(),
                                  normal.end());

        indices_index.push_back(j + of);
        j++;
        vertex.clear();
      }

      _indices_internal.push_back(indices_index[3]);
      _indices_internal.push_back(indices_index[1]);
      _indices_internal.push_back(indices_index[0]);

      _indices_internal.push_back(indices_index[0]);
      _indices_internal.push_back(indices_index[2]);
      _indices_internal.push_back(indices_index[3]);

      indices_index.clear();
      of += 4;
    }
  }

  void load(std::string file) {
    _vertices = {0.0, 0.0, 0.0};
    _colour = {
            1.0, 1.0, 1.0, // default
            .80, 0.0027, 0.038, // grid
            0.0, 1.0, 0.0 // table
    };
    _normals = {0.0, 0.0, 0.0};

    std::fstream stream(file);
    if (stream.is_open()) {
      std::string line;
      while (getline(stream, line)) {
        if (line[0] == 'v')
          parse_vertex(line);
        else if (line[0] == 'v' && line[1] == 'n')
          parse_normal(line);
        else if (line[0] == 'f')
          parse_face(line);
      }
    }
  }

  void parse_face(std::string &line) {
    std::vector<int> index;
    std::vector<int> normal;
    std::vector<int> texture;

    std::vector<std::vector<int>> face = get_face(line);
    for (auto vertex: face) {
      index.push_back(vertex[0]);
      texture.push_back(vertex[1]);
      normal.push_back(vertex[2]);
    }

    Face f = {{index[0], index[1], index[2], index[2]}, (uint) texture[0],
              (uint) normal[0]};
    _faces.push_back(f);
  }

  void parse_normal(std::string &line) {
    auto vertex_normal = get_vec3(line);
    _normals.push_back(vertex_normal.x);
    _normals.push_back(vertex_normal.y);
    _normals.push_back(vertex_normal.z);
  }

  void parse_vertex(std::string &line) {
    auto vertex = get_vec3(line);
    _vertices.push_back(vertex.x);
    _vertices.push_back(vertex.y);
    _vertices.push_back(vertex.z);
  }

  glm::vec3 get_vec3(std::string &line) const {
    std::string x{};
    std::string y{};
    std::string z{};

    int index = 0;
    for (char c: line) {
      if (c == ' ') index++;
      switch (index) {
        case 0:
          break;
        case 1:
          x += c;
          break;
        case 2:
          y += c;
          break;
        case 3:
          z += c;
          break;
        default:
          throw "in valid line";
      }
    }
    return glm::vec3(std::stof(x), std::stof(y), std::stof(z));
  }

  std::vector<std::vector<int>> get_face(std::string &line) const {
    std::string vertex_index;
    std::string texture_index;
    std::string normal_index;
    std::vector<std::vector<int>> vertex;

    int section = 0;
    int index = 0;
    for (char c: line) {
      if (c == ' ') {
        if (section != 0) {
          insert_face(vertex_index, texture_index, normal_index, vertex);
        }
        section++;
        index = 0;
        continue;
      }

      if (c == '/') {
        (index++) % 2;
        continue;
      };

      if (section == 0) continue;

      if (index == 0)
        vertex_index.push_back(c);
      else if (index == 1)
        texture_index.push_back(c);
      else if (index == 2)
        normal_index.push_back(c);
    }

    insert_face(vertex_index, texture_index, normal_index, vertex);

    return vertex;
  }

  void insert_face(std::string &vertex_index, std::string &texture_index,
                   std::string &normal_index,
                   std::vector<std::vector<int>> &vertex) const {

    // we may not have texture or normal index however we should always have vertex index
    if (texture_index.empty()) texture_index.push_back('0');
    if (normal_index.empty()) normal_index.push_back('0');

    int ti = std::stoi(texture_index);
    int vi = std::stoi(vertex_index);
    int ni = std::stoi(normal_index);

    std::vector<int> vert = {vi, ti, ni};
    vertex.push_back(vert);

    vertex_index.clear();
    texture_index.clear();
    normal_index.clear();
  }
};

class MemoryController {
public:
  Transformation transformation{{0, 0, 0}, {0, 0, 0}, 1};

  MemoryController() = default;

  ~MemoryController() {
    for (RenderObject *obj: collection) {
      delete obj;
    }
  };

  void add(RenderObject *object) { collection.push_back(object); };

  RenderObject *operator[](int index) {
    return collection[index];
  }

  std::pair<RenderObject *, glm::mat4> get(int index) {
    glm::mat4 scene_translate =
            glm::translate(glm::mat4(1.0f), {transformation.position});
    glm::mat4 scene_rotate_x = glm::rotate(glm::mat4(1.0f),
                                           transformation.rotation.x,
                                           glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 scene_rotate_y = glm::rotate(glm::mat4(1.0f),
                                           transformation.rotation.y,
                                           glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 scene_rotate_z = glm::rotate(glm::mat4(1.0f),
                                           transformation.rotation.z,
                                           glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 scene_scale = glm::scale(glm::mat4(1.0f),
                                       glm::vec3(transformation.scale,
                                                 transformation.scale,
                                                 transformation.scale));
    glm::mat4 scene =
            scene_translate * scene_rotate_x * scene_rotate_y * scene_rotate_z
            * scene_scale;

    glm::mat4 object_translate = glm::translate(glm::mat4(1.0f),
                                                {collection[index]->transformation.position});
    glm::mat4 object_rotate_x = glm::rotate(glm::mat4(1.0f),
                                            collection[index]->transformation.rotation.x,
                                            glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 object_rotate_y = glm::rotate(glm::mat4(1.0f),
                                            collection[index]->transformation.rotation.y,
                                            glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 object_rotate_z = glm::rotate(glm::mat4(1.0f),
                                            collection[index]->transformation.rotation.z,
                                            glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 object_scale = glm::scale(glm::mat4(1.0f),
                                        glm::vec3(collection[index]->transformation.scale,
                                                  collection[index]->transformation.scale,
                                                  collection[index]->transformation.scale));
    glm::mat4 object =
            object_translate * object_rotate_x * object_rotate_y * object_rotate_z
            * object_scale;

    return {collection[index], scene * object};
  }

  uint size() { return collection.size(); };

private:
  std::vector<RenderObject *> collection{};
};

class Shader {
public:
  Shader(std::string &vertxFile, std::string &fragFile) {
    loadVertexShader(vertxFile);
    loadFragShader(fragFile);
    loadProgramShader();
  };

  ~Shader() = default;

  unsigned int programShader() const { return _programShader; };

private:
  unsigned int _vertexShader;
  unsigned int _fragShader;
  unsigned int _programShader;

  static char *loadFile(std::string &file) {
    FILE *fp = fopen(file.c_str(), "r");

    if (fp == NULL) { return NULL; }

    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);

    fseek(fp, 0L, SEEK_SET);
    char *buf = new char[size + 1];
    fread(buf, 1, size, fp);

    buf[size] = '\0';
    fclose(fp);
    return buf;
  };

  void loadVertexShader(std::string &file) {
    int success;
    char infoLog[512];
    char *vertexShaderSource = loadFile(file);
    _vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(_vertexShader,
                   1,
                   (const GLchar **) &vertexShaderSource,
                   nullptr);
    glCompileShader(_vertexShader);

    glGetShaderiv(_vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(_vertexShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog
                << std::endl;
    };
  };

  void loadFragShader(std::string &file) {
    int success;
    char infoLog[512];
    const char *fragmentShaderSource = loadFile(file);
    _fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(_fragShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(_fragShader);

    glGetShaderiv(_fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(_fragShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::FRAG::COMPILATION_FAILED\n" << infoLog
                << std::endl;
    };

  };

  void loadProgramShader() {
    int success;
    char infoLog[512];

    _programShader = glCreateProgram();
    glAttachShader(_programShader, _vertexShader);
    glAttachShader(_programShader, _fragShader);
    glLinkProgram(_programShader);
    glGetProgramiv(_programShader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(_programShader, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog
                << std::endl;
    }
    glCheckError();
  };
};

struct State {
  int width = 500;
  int height = 500;

  bool camera_control_mouse = true;
  bool camera_control_keyboard = true;

  bool firstMouse = true;
  float lastX = {};
  float lastY = {};
  float yaw = -90;
  float pitch = {};
  Camara camara{{0.45, 0.53, 0.82}, {0, -0.32, -0.95}, {0, 1, 0}, perspective};

  uint collection_index = 0;
  MemoryController collection;
  Shader *shader;
};

State state;

// keeps the dragon on the table
void so_called_physics() {
  RenderObject *dragon = state.collection[0];
  RenderObject *table = state.collection[2];

  float diff = dragon->transformation.position.z - table->transformation.position.z;
  if (diff < -.090f)
    dragon->transformation.position.z =
        -.090f + table->transformation.position.z;
  if (diff > .075f)
    dragon->transformation.position.z =
        .075f + table->transformation.position.z;
}

void display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCheckError();

  glm::mat4 view = glm::lookAt(state.camara.position,
                               state.camara.position + state.camara.target,
                               state.camara.up);
  glm::mat4 proj = glm::mat4(1.f);

  if (state.camara.projectionType == perspective) {
    proj = glm::perspective<float>(glm::radians(45.0f),
                                   (float) state.width / (float) state.height,
                                   0.1f,
                                   100.0f);
  } else if (state.camara.projectionType == orthographic) {
    proj = glm::ortho<float>(-1, 1, -1, 1, -1, 100);
  }

  so_called_physics();

  for (int i = 0; i < state.collection.size(); i++) {
    if (!state.collection[i]->draw) continue;
    auto [object, matrix] = state.collection.get(i);

    if (object->wireframe_mode) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    GLint viewLoc = glGetUniformLocation(state.shader->programShader(), "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    GLint perLoc = glGetUniformLocation(state.shader->programShader(), "per");
    glUniformMatrix4fv(perLoc, 1, GL_FALSE, glm::value_ptr(proj));

    GLint transformLoc =
            glGetUniformLocation(state.shader->programShader(), "model");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matrix));

    glBindVertexArray(object->getVertexArrayObject());
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->getElementBufferObjets());
    glDrawElements(GL_TRIANGLES,
                   object->numberOfIndices(),
                   GL_UNSIGNED_INT,
                   nullptr);
    glBindVertexArray(0);
    glCheckError();
  }

  glCheckError();
  glutSwapBuffers();
}

void reshape(int width, int height) {
  state.width = width;
  state.height = height;
  glViewport(0, 0, width, height);
  glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
  //std::cout << (int)key << ", " << x << ", " << y << '\n';

  // state
  if (key == KEY::ESC) exit(0);
  if (key == KEY::Z) state.camera_control_mouse = !state.camera_control_mouse;
  if (key == KEY::X)
    state.camera_control_keyboard = !state.camera_control_keyboard;

  // camera
  const float cameraSpeed = 0.05f;
  if (key == KEY::W)
    state.camara.position += cameraSpeed * state.camara.target;
  if (key == KEY::S)
    state.camara.position -= cameraSpeed * state.camara.target;
  if (key == KEY::A)
    state.camara.position -=
            glm::normalize(glm::cross(state.camara.target, state.camara.up))
            * cameraSpeed;
  if (key == KEY::D)
    state.camara.position +=
            glm::normalize(glm::cross(state.camara.target, state.camara.up))
            * cameraSpeed;
  if (key == KEY::Q)
    state.camara.position += state.camara.up * cameraSpeed;
  if (key == KEY::E)
    state.camara.position -= state.camara.up * cameraSpeed;

  if (key == KEY::P) state.camara.projectionType = perspective;
  if (key == KEY::O) state.camara.projectionType = orthographic;

  if (key == KEY::R) state.camara.print();

  // object
  if (key == KEY::Y)
    state.collection[state.collection_index]->transformation.scale *= 1.5f;
  if (key == KEY::I)
    state.collection[state.collection_index]->transformation.scale *= 1 / 1.5f;

  if (key == KEY::U)
    state.collection[state.collection_index]->transformation.rotation.x += 1.0f;
  if (key == KEY::J)
    state.collection[state.collection_index]->transformation.rotation.x -= 1.0f;

  if (key == KEY::H)
    state.collection[state.collection_index]->transformation.rotation.y += 1.0f;
  if (key == KEY::K)
    state.collection[state.collection_index]->transformation.rotation.y -= 1.0f;

  if (key == KEY::SLASH)
    state.collection[state.collection_index]->wireframe_mode =
            !state.collection[state.collection_index]->wireframe_mode;

  if (key == KEY::L)
    state.collection[state.collection_index]->draw =
            !state.collection[state.collection_index]->draw;

  if (key == KEY::T) state.collection[state.collection_index]->print();

  // collection
  if (key == KEY::M) state.collection.transformation.scale *= 1.5f;
  if (key == KEY::N) state.collection.transformation.scale *= 1 / 1.5f;
  if (key == KEY::COMMA)
    state.collection_index = (state.collection_index - 1)
                             % (state.collection.size());
  if (key == KEY::PERIOD)
    state.collection_index = (state.collection_index + 1)
                             % (state.collection.size());

  if (key == KEY::ONE) state.collection_index = 0;
  if (key == KEY::TWO) state.collection_index = 1;
  if (key == KEY::THREE) state.collection_index = 2;

  glutPostRedisplay();
}

void specialKeys(int key, int x, int y) {
  // state
  // camera
  if (key == GLUT_KEY_F1) {
    state.camara.position = {0.45, 0.53, 0.82};
    state.camara.target = {0, -0.32, -0.95};
  }

  if (key == GLUT_KEY_F2) {
    state.camara.position = {0.45, 0.53, -1.82};
    state.camara.target = {0, -0.32, 0.95};
  }

  // object
  RenderObject *object = state.collection[state.collection_index];
  if (key == GLUT_KEY_HOME) object->transformation.rotation.x += 1.0f;
  if (key == GLUT_KEY_END) object->transformation.rotation.y += 1.0f;
  if (key == GLUT_KEY_INSERT) object->transformation.rotation.z += 1.0f;

  RenderObject *table = state.collection[2];

  if (key == GLUT_KEY_UP) {
    object->transformation.position.z += 0.01f;

  }
  if (key == GLUT_KEY_DOWN) {
    object->transformation.position.z -= 0.01f;
  }
  if (key == GLUT_KEY_LEFT) object->transformation.rotation.y += glm::radians(90.0f);
  if (key == GLUT_KEY_RIGHT) object->transformation.rotation.y -= glm::radians(90.0f);

  //collection
  if (key == GLUT_KEY_PAGE_UP) state.collection.transformation.scale *= 1.5f;
  if (key == GLUT_KEY_PAGE_DOWN)
    state.collection.transformation.scale *= 1 / 1.5f;

  glutPostRedisplay();
}

void mouse(int button, int s, int xpos, int ypos) {
  if (button == 0) state.firstMouse = true;
}

void mousedrag(int xpos, int ypos) {
  if (!state.camera_control_mouse) return;
  if (state.firstMouse) {
    state.lastX = xpos;
    state.lastY = ypos;
    state.firstMouse = false;
  }

  float xoffset = xpos - state.lastX;
  float yoffset = state.lastY - ypos;
  state.lastX = xpos;
  state.lastY = ypos;

  float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  state.yaw += xoffset;
  state.pitch += yoffset;

  if (state.pitch > 89.0f)
    state.pitch = 89.0f;
  if (state.pitch < -89.0f)
    state.pitch = -89.0f;

  glm::vec3 direction;
  direction.x = cos(glm::radians(state.yaw)) * cos(glm::radians(state.pitch));
  direction.y = sin(glm::radians(state.pitch));
  direction.z = sin(glm::radians(state.yaw)) * cos(glm::radians(state.pitch));
  state.camara.target = glm::normalize(direction);
  glutPostRedisplay();

  // keeps the mouse pointer in the center of the window when dragging
  if (xpos < 10 || xpos > state.width - 10) {
    state.lastX = state.width / 2;
    state.lastY = state.height / 2;
    glutWarpPointer(state.width / 2, state.height / 2);
  } else if (ypos < 10 || ypos > state.height - 10) {
    state.lastX = state.width / 2;
    state.lastY = state.height / 2;
    glutWarpPointer(state.width / 2, state.height / 2);
  }

}

void init() {
  std::string working_dir = "./";
  glViewport(0, 0, state.width, state.height);

  std::string vertFile = working_dir + "shaders/shader.vert";
  std::string fragFile = working_dir + "shaders/shader.frag";
  state.shader = new Shader(vertFile, fragFile);

  int shad = state.shader->programShader();
  glUseProgram(shad);
  glCheckError();

  auto *dragon = new RenderObject();
  dragon->generate(working_dir + "Dragon.obj");
  dragon->transformation.scale *= 0.00118519f;
  dragon->transformation.position.x = 0.45f;
  dragon->transformation.position.y = 0.165f;
  dragon->transformation.position.z = -0.43;
  state.collection.add(dragon);

  auto *grid = new RenderObject();
  grid->generate(working_dir + "Grid.obj");
  grid->transformation.scale *= 0.05f;
  state.collection.add(grid);

  auto *table = new RenderObject();
  table->generate(working_dir + "table_colour.obj");
  table->transformation.scale *= 0.1f;
  table->transformation.position.x = 0.45f;
  table->transformation.position.z = -0.45f;
  table->transformation.position.y = 0.01f;
  state.collection.add(table);

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
  //glDepthFunc(GL_LEQUAL);
  //glDepthMask(GL_TRUE);
  glEnable(GL_DEPTH_TEST);
  //glDepthRange(0, -1);
  glCheckError();
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(state.width, state.height);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Final");
  glewInit();
  init();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMotionFunc(mousedrag);
  glutSpecialFunc(specialKeys);
  glutMainLoop();

  delete state.shader;
  std::cout << "goodbye, World!" << std::endl;
  return 0;
}
