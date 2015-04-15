#pragma once

#include "stdafx.h"


void write(FileStorage& fs, const string&, const KeyFrame& keyframe);
void read(const FileNode& node, KeyFrame& keyframe, const KeyFrame& default_value);

void write(FileStorage& fs, const string&, const CloudPoint& cp);
void read(const FileNode& node, CloudPoint& cp, const CloudPoint& default_value);


void write(FileStorage& fs, const string&, const MyKeyPoint& m);

void read(const FileNode& node, MyKeyPoint& m, const MyKeyPoint& default_value = MyKeyPoint());

void write(FileStorage& fs, const string&, const vector<MyKeyPoint>& m);




void read(const FileNode& node, vector<MyKeyPoint>& m, const vector<MyKeyPoint>& default_value = vector<MyKeyPoint>());

void write(FileStorage& fs, const string&, const vector<KeyFrame>& m);




void read(const FileNode& node, vector<KeyFrame>& m, const vector<KeyFrame>& default_value = vector<KeyFrame>());

void write(FileStorage& fs, const string&, const vector<CloudPoint>& m);



void read(const FileNode& node, vector<CloudPoint>& m, const vector<CloudPoint>& default_value = vector<CloudPoint>());