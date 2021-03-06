#pragma once

#include "Observables/ObservableCollection.h"

#include <vector>

using namespace std;

vector<double> operator*(vector<int> v, double x);

vector<double>& operator+=(vector<double>& v1, const vector<double>& v2);
vector<double>& operator+=(vector<double>& v1, double d);
vector<double>& operator-=(vector<double>& v1, const vector<double>& v2);
vector<double>& operator-=(vector<double>& v1, double d);
vector<double>& operator*=(vector<double>& v, double x);
vector<double>& operator/=(vector<double>& v, double x);
vector<double> operator+(vector<double> v1, const vector<double>& v2);
vector<double> operator+(vector<double> v1, double d);
vector<double> operator-(vector<double> v1, const vector<double>& v2);
vector<double> operator-(vector<double> v1, double d);
vector<double> operator*(vector<double> v, double x);
vector<double> operator/(vector<double> v, double x);

vector<vector<double> >& operator+=(vector<vector<double> >& v1, const vector<vector<double> >& v2);
vector<vector<double> >& operator*=(vector<vector<double> >& v, double x);
vector<vector<double> >& operator/=(vector<vector<double> >& v, double x);
vector<vector<double> > operator+(vector<vector<double> > v1, const vector<vector<double> >& v2);
vector<vector<double> > operator*(vector<vector<double> > v, double x);
vector<vector<double> > operator/(vector<vector<double> > v, double x);

Observables::ObservableCollection operator+(Observables::ObservableCollection lhs, const Observables::ObservableCollection& rhs);
Observables::ObservableCollection operator-(Observables::ObservableCollection lhs, const Observables::ObservableCollection& rhs);
Observables::ObservableCollection operator*(Observables::ObservableCollection lhs, double rhs);
Observables::ObservableCollection operator/(Observables::ObservableCollection lhs, double rhs);
