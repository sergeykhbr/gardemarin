#pragma once

class ISimFace {
 public:
    ISimFace(const char *name) : face_name_(name) {}
    virtual const char *getFaceName() { return face_name_; }
 private:
    const char *face_name_;
};
