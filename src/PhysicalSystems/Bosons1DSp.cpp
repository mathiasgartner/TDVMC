#include "Bosons1DSp.h"

#include "../SplineFactory.h"

namespace PhysicalSystems
{

Bosons1DSp::Bosons1DSp(vector<double>& params, string configDirectory) :
		IPhysicalSystem(params, configDirectory)
{
	this->USE_NORMALIZATION_AND_PHASE = true;
	this->USE_NIC = true;
	this->USE_MOVE_COM_TO_ZERO = false;

	splineOrder = 3;
	numberOfSplines = 0;

	halfLength = 0;
	maxDistance = 0;
	numberOfSpecialParametersStart = 0;
	numberOfSpecialParametersEnd = 0;
	numberOfStandardParameters = 0;
	np1 = 0;
	np2 = 0;
	np3 = 0;

	numOfOtherLocalOperators = 0;
	numOfkValues = 0;

	exponentNew = 0;
	changedParticleIndex = 0;
}

void Bosons1DSp::SetNodes(vector<double> n)
{
	this->nodes = n;
	int additionalNodeCount = splineOrder;
	for (int i = 0; i < additionalNodeCount; i++)
	{
		this->nodes.insert(this->nodes.begin(), -n[i + 1]);
		this->nodes.push_back(2.0 * n[n.size() - 1] - n[n.size() - 2 - i]);
	}
}

void Bosons1DSp::InitSystem()
{
	numOfOtherLocalOperators = 4; //INFO: potentialIntern, potentialInternComplex, potentialExtern, potentialExternComplex
	otherLocalOperators.resize(numOfOtherLocalOperators);
	numOfkValues = 50;
	numOfOtherExpectationValues = 9;
	numOfAdditionalSystemProperties = numOfOtherExpectationValues;

	//INFO: BC
	numberOfSplines = nodes.size() - splineOrder - 1; //dth order splines: -(d+1)
	halfLength = LBOX / 2.0;
	maxDistance = nodes[nodes.size() - splineOrder - 1];
	splineWeights = SplineFactory::GetWeights(nodes);

	//cut off
	if (splineOrder == 3)
	{
		SplineFactory::SetBoundaryConditions3_1D_OR_1(nodes, bcFactorsStart);
		SplineFactory::SetBoundaryConditions3_1D_CO_2(nodes, bcFactorsEnd, maxDistance);
	}
	else if (splineOrder == 4)
	{

	}
	numberOfSpecialParametersStart = bcFactorsStart.size();
	numberOfSpecialParametersEnd = bcFactorsEnd.size();
	numberOfStandardParameters = N_PARAM - numberOfSpecialParametersStart - numberOfSpecialParametersEnd;
	np1 = numberOfSpecialParametersStart;
	np2 = np1 + numberOfStandardParameters;
	np3 = np2 + numberOfSpecialParametersEnd;

	if (N_PARAM != numberOfSplines - (splineOrder - numberOfSpecialParametersStart) - (splineOrder - numberOfSpecialParametersEnd))
	{
		cout << "!!! WRONG NUMBER OF PARAMETERS !!!" << endl;
	}

	wf = 0.0;
	exponent = 0.0;
	InitVector(splineSums, numberOfSplines, 0.0);
	InitVector(splineSumsD, numberOfSplines, N, DIM, 0.0);
	InitVector(splineSumsD2, numberOfSplines, N, 0.0);

	localEnergyR = 0;
	localEnergyI = 0;
	InitVector(localOperators, N_PARAM, 0.0);
	InitVector(localOperatorsMatrix, N_PARAM, N_PARAM, 0.0);
	InitVector(localOperatorlocalEnergyR, N_PARAM, 0.0);
	InitVector(localOperatorlocalEnergyI, N_PARAM, 0.0);
	InitVector(otherExpectationValues, numOfOtherExpectationValues, 0.0);
	InitVector(additionalSystemProperties, numOfAdditionalSystemProperties, 0.0);

	wfNew = 0.0;
	exponentNew = 0.0;
	InitVector(splineSumsNew, numberOfSplines, 0.0);
	InitVector(sumOldPerBin, numberOfSplines, 0.0);
	InitVector(sumNewPerBin, numberOfSplines, 0.0);

	kValues = ReadKValuesFromJsonFile(this->configDirectory + "kVectors" + to_string(DIM) + "D.json");
	ReadDataFromFile(kNorms, this->configDirectory + "kNorm" + to_string(DIM) + "D.csv");
	kNorms.resize(numOfkValues);
	for (int k = 0; k < numOfkValues; k++)
	{
		for (unsigned int kn = 0; kn < kValues[k].size(); kn++)
		{
			for (int a = 0; a < DIM; a++)
			{
				kValues[k][kn][a] *= 2 * M_PI / LBOX;
			}
		}
		kNorms[k] *= 2 * M_PI / LBOX;
	}

	//cout << "maxDistance=" << maxDistance << endl;
	//cout << "nodePointSpacing=" << nodePointSpacing << endl;

	pairDistribution.name = "pairDistribution";
	//pairDistribution.InitGrid(0.0, halfLength, 0.05);
	pairDistribution.InitGrid(0.0, nodes[nodes.size() - 4], nodes[nodes.size() - 4] / 100.0);
	pairDistribution.InitScaling();
	pairDistribution.InitObservables( { "g_2(r_ij)" });

	structureFactor.name = "structureFactor";
	structureFactor.InitGrid(kNorms);
	structureFactor.InitObservables( { "S(k)" });

	additionalObservables.Add(&pairDistribution);
	additionalObservables.Add(&structureFactor);
}

void Bosons1DSp::RefreshLocalOperators()
{
	//INFO: BC
	for (int i = 0; i < np1; i++)
	{
		double tmp = 0.0;
		for (int j = 0; j < splineOrder; j++)
		{
			tmp += bcFactorsStart[i][j] * splineSums[j];
		}
		this->localOperators[i] = tmp;
	}
	int spIdx = splineOrder;
	for (int i = np1; i < np2; i++)
	{
		this->localOperators[i] = splineSums[spIdx];
		spIdx++;
	}
	int idx = 0;
	spIdx = numberOfSplines - splineOrder;
	for (int i = np2; i < np3; i++)
	{
		double tmp = 0.0;
		for (int j = 0; j < splineOrder; j++)
		{
			tmp += bcFactorsEnd[idx][j] * splineSums[spIdx];
		}
		this->localOperators[i] = tmp;
		idx++;
		spIdx++;
	}
}

void Bosons1DSp::CalculateLocalOperators(vector<vector<double> >& R)
{
	int bin;
	double rni;
	vector<double> rniPowers(splineOrder + 1);
	vector<double> vecrni(DIM);

	ClearVector(splineSums);

	for (int n = 0; n < N; n++)
	{
		for (int i = 0; i < n; i++)
		{
			rni = VectorDisplacementNIC_DIM(R[n], R[i], vecrni);
			if (rni < maxDistance)
			{
				auto interval = lower_bound(nodes.begin(), nodes.end(), rni);
				bin = interval - nodes.begin() - 1;
				rniPowers[0] = 1.0;
				rniPowers[1] = rni;
				for (int j = 2; j < splineOrder + 1; j++)
				{
					rniPowers[j] = rniPowers[j - 1] * rni;
				}

				for (int p = 0; p < splineOrder + 1; p++)
				{
					for (int j = 0; j < splineOrder + 1; j++)
					{
						splineSums[bin - p] += splineWeights[bin - p][p][j] * rniPowers[j];
					}
				}
			}
			else
			{
				cout << "!!!!" << endl;
			}
		}
	}
	RefreshLocalOperators();
}

void Bosons1DSp::CalculateOtherLocalOperators(vector<vector<double> >& R)
{
	int s = splineOrder + 1;
	vector<double> vecrni(DIM);
	vector<double> evecrni(DIM);
	vector<double> tmp1(s);
	vector<double> tmp2(s);
	int bin;
	double rni;
	vector<double> rniPowers(splineOrder);

	double potentialExtern = 0;
	double potentialExternComplex = 0;
	double potentialIntern = 0;
	double potentialInternComplex = 0;

	//potential parameters (a -> width; b -> height)
	double a = params[0];
	double b = params[1];
	if (params.size() > 2 && this->time >= params[2])
	{
		a = params[3];
		b = params[4];
	}

	ClearVector(splineSumsD);
	ClearVector(splineSumsD2);
	ClearVector(otherLocalOperators);

	for (int n = 0; n < N; n++)
	{
		//external potential energy
		potentialExtern += GetExternalPotential(R[n]);

		for (int i = 0; i < N; i++)
		{
			rni = VectorDisplacementNIC_DIM(R[n], R[i], vecrni);

			if (rni < maxDistance)
			{
				//internal potential energy
				if (i < n)
				{
					//Gauss
					//double rnia = rni / a;
					//potentialIntern += b * exp(-(rnia * rnia) / 2.0);

					//square well
					if (rni < a)
					{
						potentialIntern += b;
					}

					//Rydberg U_0 / (1 + (r/R_0)^6))
					//R_0 = a, U_0 = b;
					//double rniR0 = rni / a;
					//double rniR03 = rniR0 * rniR0 * rniR0;
					//potentialIntern += b / (1 + rniR03 * rniR03);

					//imaginary part
					//double rniAbsorption = rni - 4.5;
					//double absorptionFactor = 0.0;//-3e-6;
					//if (rniAbsorption > 0)
					//{
					//	potentialInternComplex += absorptionFactor * rniAbsorption * rniAbsorption;
					//}
				}

				//kinetic energy
				//TODO: maybe it is faster to calculate all localOperators[i][n] before and then loop over this precalculated values
				//		otherwise values are calculated multiple times
				if (i != n) //TODO: also include in (i < n) branch -> then only loop over i < n in "for (int i = 0; i < N; i++)"
				{
					auto interval = lower_bound(nodes.begin(), nodes.end(), rni);
					bin = interval - nodes.begin() - 1;
					rniPowers[0] = 1.0;
					rniPowers[1] = rni;
					for (int j = 2; j < splineOrder; j++)
					{
						rniPowers[j] = rniPowers[j - 1] * rni;
					}

					for (int p = 0; p < splineOrder + 1; p++)
					{
						tmp1[3 - p] = 0.0;
						tmp2[3 - p] = 0.0;
						for (int j = 1; j < splineOrder + 1; j++)
						{
							//first derivative
							tmp1[3 - p] += j * splineWeights[bin - p][p][j] * rniPowers[j - 1];
						}
						for (int j = 2; j < splineOrder + 1; j++)
						{
							//second derivative
							tmp2[3 - p] = j * (j - 1) * splineWeights[bin - p][p][j] * rniPowers[j - 2];
						}
					}

					for (int a = 0; a < DIM; a++)
					{
						evecrni[a] = vecrni[a] / rni;
					}
					bool outsideL2 = false;
					double firstDerivativeFactor = outsideL2 ? -1.0 : 1.0;
					for (int a = 0; a < DIM; a++)
					{
						for (int b = 0; b < s; b++)
						{
							splineSumsD[bin - b][n][a] += tmp1[splineOrder - b] * evecrni[a] * firstDerivativeFactor;
						}
					}
					double secondDerivativeFactor = DIM - 1.0; //INFO: at least correct for 2D and 3D (and 1D?)
					for (int b = 0; b < s; b++)
					{
						splineSumsD2[bin - b][n] += tmp2[splineOrder - b] + secondDerivativeFactor / rni * tmp1[splineOrder - b];
					}
				}
			}
		}
	}
	otherLocalOperators[0] = potentialIntern;
	otherLocalOperators[1] = potentialInternComplex;
	otherLocalOperators[2] = potentialExtern;
	otherLocalOperators[3] = potentialExternComplex;
}

vector<double> Bosons1DSp::GetCenterOfMass(vector<vector<double> >& R)
{
	vector<double> com = { 0.0, 0.0, 0.0 };
	return com;
}

double Bosons1DSp::GetExternalPotential(vector<double>& r)
{
	return 0;
}

void Bosons1DSp::CalculateExpectationValues(vector<double>& O, vector<vector<vector<double> > >& sD, vector<vector<double> >& sD2, vector<double>& otherO, vector<double>& gr, vector<double>& uR, vector<double>& uI, double phiR, double phiI)
{
	double tmp = 0;
	double kineticR = 0;
	double kineticI = 0;
	vector<double> vecKineticSumR1(DIM);
	vector<double> vecKineticSumI1(DIM);
	double kineticSumR1 = 0;
	double kineticSumI1 = 0;
	double kineticSumR1I1 = 0;
	double kineticSumR2 = 0;
	double kineticSumI2 = 0;

	localEnergyR = 0;
	localEnergyI = 0;

	ClearVector(localOperatorsMatrix);
	ClearVector(localOperatorlocalEnergyR);
	ClearVector(localOperatorlocalEnergyI);
	ClearVector(otherExpectationValues);

	for (int n = 0; n < N; n++)
	{
		for (int a = 0; a < DIM; a++)
		{
			vecKineticSumR1[a] = 0.0;
			vecKineticSumI1[a] = 0.0;
		}

		//INFO: BC
		for (int i = 0; i < np1; i++)
		{
			for (int a = 0; a < DIM; a++)
			{
				tmp = 0;
				for (int j = 0; j < splineOrder; j++)
				{
					tmp += bcFactorsStart[i][j] * sD[j][n][a];
				}
				vecKineticSumR1[a] += uR[i] * tmp;
				vecKineticSumI1[a] += uI[i] * tmp;
			}
			tmp = 0;
			for (int j = 0; j < splineOrder; j++)
			{
				tmp += bcFactorsStart[i][j] * sD2[j][n];
			}
			kineticSumR2 += uR[i] * tmp;
			kineticSumI2 += uI[i] * tmp;
		}
		int spIdx = splineOrder;
		for (int i = np1; i < np2; i++)
		{
			for (int a = 0; a < DIM; a++)
			{
				vecKineticSumR1[a] += uR[i] * sD[spIdx][n][a];
				vecKineticSumI1[a] += uI[i] * sD[spIdx][n][a];
			}
			kineticSumR2 += uR[i] * sD2[spIdx][n];
			kineticSumI2 += uI[i] * sD2[spIdx][n];
			spIdx++;
		}
		int idx = 0;
		spIdx = numberOfSplines - splineOrder;
		for (int i = np2; i < np3; i++)
		{
			for (int a = 0; a < DIM; a++)
			{
				tmp = 0;
				for (int j = 0; j < splineOrder; j++)
				{
					tmp += bcFactorsEnd[idx][j] * sD[spIdx][n][a];
				}
				vecKineticSumR1[a] += uR[i] * tmp;
				vecKineticSumI1[a] += uI[i] * tmp;
			}
			tmp = 0;
			for (int j = 0; j < splineOrder; j++)
			{
				tmp += bcFactorsEnd[idx][j] * sD2[spIdx][n];
			}
			kineticSumR2 += uR[i] * tmp;
			kineticSumI2 += uI[i] * tmp;
			idx++;
		}

		kineticSumR1I1 += 2.0 * VectorDotProduct_DIM(vecKineticSumR1, vecKineticSumI1);
		kineticSumR1 += VectorNorm2_DIM(vecKineticSumR1);
		kineticSumI1 += VectorNorm2_DIM(vecKineticSumI1);
	}

	kineticR = -(kineticSumR1 - kineticSumI1 + kineticSumR2);
	kineticI = -(kineticSumR1I1 + kineticSumI2);

	localEnergyR = kineticR + otherO[0] + otherO[2];
	localEnergyI = kineticI + otherO[1];

	//cout << "potentialExtern=" << potentialExtern << endl;
	//cout << "potentialIntern=" << potentialIntern << endl;
	//cout << "kineticSumR1=" << kineticSumR1 << endl;
	//cout << "kineticSumI1=" << kineticSumI1 << endl;
	//cout << "kineticSumR2=" << kineticSumR2 << endl;
	//cout << "kineticSumI2=" << kineticSumI2 << endl;
	//cout << "kineticSumR1I1=" << kineticSumR1I1 << endl;
	//cout << "kineticR=" << kineticR << endl;

	localOperators = O; //INFO: for providing the localOperators to TDVMC.cpp via IPhysicalSystem.GetLocalOperators()
	for (int k = 0; k < N_PARAM; k++)
	{
		for (int j = 0; j < N_PARAM; j++)
		{
			localOperatorsMatrix[k][j] = O[k] * O[j];
		}
		localOperatorlocalEnergyR[k] = O[k] * localEnergyR;
		localOperatorlocalEnergyI[k] = O[k] * localEnergyI;
	}

	otherExpectationValues[0] = kineticR;
	otherExpectationValues[1] = otherO[0];
	otherExpectationValues[2] = wf;
	otherExpectationValues[3] = exponent;
	otherExpectationValues[4] = kineticSumR1;
	otherExpectationValues[5] = kineticSumI1;
	otherExpectationValues[6] = kineticSumR2;
	otherExpectationValues[7] = kineticSumI2;
	otherExpectationValues[8] = kineticSumR1I1;
}

void Bosons1DSp::CalculateExpectationValues(vector<vector<double> >& R, vector<double>& uR, vector<double>& uI, double phiR, double phiI)
{
	RefreshLocalOperators();
	CalculateOtherLocalOperators(R);
	vector<double> dummy_grBins;
	CalculateExpectationValues(this->localOperators, this->splineSumsD, this->splineSumsD2, this->otherLocalOperators, dummy_grBins, uR, uI, phiR, phiI);
}

void Bosons1DSp::CalculateExpectationValues(ICorrelatedSamplingData* sample, vector<double>& uR, vector<double>& uI, double phiR, double phiI)
{
	CSDataBulkSplines* s = dynamic_cast<CSDataBulkSplines*>(sample);
	CalculateExpectationValues(s->localOperators, s->splineSumsD, s->splineSumsD2, s->otherLocalOperators, s->grBins, uR, uI, phiR, phiI);
}

void Bosons1DSp::CalculateAdditionalSystemProperties(vector<vector<double> >& R, vector<double>& uR, vector<double>& uI, double phiR, double phiI)
{
	additionalObservables.ClearValues();
	double rni;
	vector<double> vecrni(DIM);

	//pairDistribution
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < i; j++)
		{
			rni = VectorDisplacementNIC_DIM(R[i], R[j], vecrni);
			if (rni < pairDistribution.grid.max)
			{
				pairDistribution.AddToHistogram(0, rni, 1.0);
			}
		}
	}

	//structureFactor
	vector<double> sumSCos;
	vector<double> sumSSin;
	vector<double> sk;
	InitVector(sumSCos, numOfkValues, 0.0);
	InitVector(sumSSin, numOfkValues, 0.0);
	InitVector(sk, numOfkValues, 0.0);
	for (int i = 0; i < N; i++)
	{
		for (int k = 0; k < numOfkValues; k++)
		{
			for (unsigned int kn = 0; kn < kValues[k].size(); kn++)
			{
				sumSCos[k] += cos(kValues[k][kn][0] * R[i][0] + kValues[k][kn][1] * R[i][1] + kValues[k][kn][2] * R[i][2]);
				sumSSin[k] += sin(kValues[k][kn][0] * R[i][0] + kValues[k][kn][1] * R[i][1] + kValues[k][kn][2] * R[i][2]);
			}
		}
	}
	for (int k = 0; k < numOfkValues; k++)
	{
		sk[k] = (sumSCos[k] * sumSCos[k] + sumSSin[k] * sumSSin[k]) / ((double) (N * kValues[k].size()));
		structureFactor.SetValueAtGridIndex(0, k, sk[k]);
	}
}

void Bosons1DSp::CalculateWavefunction(vector<double>& O, vector<double>& uR, vector<double>& uI, double phiR, double phiI)
{
	double sum = 0;

	for (int i = 0; i < N_PARAM; i++)
	{
		sum += uR[i] * O[i];
	}

	//cout << scientific << setprecision(16) << "sum:" << sum << endl;
	exponent = sum;
	wf = exp(exponent + phiR);
}

void Bosons1DSp::CalculateWavefunction(vector<vector<double> >& R, vector<double>& uR, vector<double>& uI, double phiR, double phiI)
{
	CalculateLocalOperators(R);
	CalculateWavefunction(this->localOperators, uR, uI, phiR, phiI);
}

void Bosons1DSp::CalculateWavefunction(ICorrelatedSamplingData* sample, vector<double>& uR, vector<double>& uI, double phiR, double phiI)
{
	CSDataBulkSplines* s = dynamic_cast<CSDataBulkSplines*>(sample);
	CalculateWavefunction(s->localOperators, uR, uI, phiR, phiI);
}

void Bosons1DSp::CalculateWFChange(vector<vector<double> >& R, vector<double>& uR, vector<double>& uI, double phiR, double phiI, int changedParticleIndex, vector<double>& oldPosition)
{
	double sum = 0;
	int bin;
	double rni;
	vector<double> rniPowers(splineOrder + 1);
	vector<double> vecrni(DIM);

	ClearVector(sumOldPerBin);
	ClearVector(sumNewPerBin);
	for (int i = 0; i < N; i++)
	{
		if (i != changedParticleIndex)
		{
			rni = VectorDisplacementNIC_DIM(R[i], oldPosition, vecrni);
			if (rni < maxDistance)
			{
				auto interval = lower_bound(nodes.begin(), nodes.end(), rni);
				bin = interval - nodes.begin() - 1;
				rniPowers[0] = 1.0;
				rniPowers[1] = rni;
				for (int j = 2; j < splineOrder + 1; j++)
				{
					rniPowers[j] = rniPowers[j - 1] * rni;
				}

				for (int p = 0; p < splineOrder + 1; p++)
				{
					for (int j = 0; j < splineOrder + 1; j++)
					{
						sumOldPerBin[bin - p] += splineWeights[bin - p][p][j] * rniPowers[j];
					}
				}
			}
			else
			{
				cout << "!!!" << endl;
			}
			rni = VectorDisplacementNIC_DIM(R[i], R[changedParticleIndex], vecrni);
			if (rni < maxDistance)
			{
				auto interval = lower_bound(nodes.begin(), nodes.end(), rni);
				bin = interval - nodes.begin() - 1;
				rniPowers[0] = 1.0;
				rniPowers[1] = rni;
				for (int j = 2; j < splineOrder + 1; j++)
				{
					rniPowers[j] = rniPowers[j - 1] * rni;
				}

				for (int p = 0; p < splineOrder + 1; p++)
				{
					for (int j = 0; j < splineOrder + 1; j++)
					{
						sumNewPerBin[bin - p] += splineWeights[bin - p][p][j] * rniPowers[j];
					}
				}
			}
			else
			{
				cout << "!!!" << endl;
			}
		}
	}

	for (int i = 0; i < numberOfSplines; i++)
	{
		splineSumsNew[i] = max(0.0, splineSums[i] - sumOldPerBin[i] + sumNewPerBin[i]);
	}

	//INFO: BC
	for (int i = 0; i < np1; i++)
	{
		double tmp = 0.0;
		for (int j = 0; j < splineOrder; j++)
		{
			tmp += bcFactorsStart[i][j] * splineSumsNew[j];
		}
		this->localOperators[i] = uR[i] * tmp;
	}
	int spIdx = splineOrder;
	for (int i = np1; i < np2; i++)
	{
		this->localOperators[i] = uR[i] * splineSumsNew[spIdx];
		spIdx++;
	}
	int idx = 0;
	spIdx = numberOfSplines - splineOrder;
	for (int i = np2; i < np3; i++)
	{
		double tmp = 0.0;
		for (int j = 0; j < splineOrder; j++)
		{
			tmp += bcFactorsEnd[idx][j] * splineSumsNew[spIdx];
		}
		this->localOperators[i] = uR[i] * tmp;
		idx++;
		spIdx++;
	}


	exponentNew = sum;
	wfNew = exp(exponentNew + phiR);
}

double Bosons1DSp::CalculateWFQuotient(vector<vector<double> >& R, vector<double>& uR, vector<double>& uI, double phiR, double phiI, int changedParticleIndex, vector<double>& oldPosition)
{
	CalculateWFChange(R, uR, uI, phiR, phiI, changedParticleIndex, oldPosition);
	double wfQuotient = exp(2.0 * (exponentNew - exponent));

	//cout << "qu=" << wfQuotient << " (" << exponentNew << " - " << exponent << ")" << endl;

	return wfQuotient;
}

void Bosons1DSp::AcceptMove()
{
	wf = wfNew;
	exponent = exponentNew;
	splineSums = splineSumsNew;
}

void Bosons1DSp::InitCorrelatedSamplingData(vector<ICorrelatedSamplingData*>& data)
{
	for (unsigned int i = 0; i < data.size(); i++)
	{
		data[i] = new CSDataBulkSplines();
	}
}

void Bosons1DSp::FillCorrelatedSamplingData(ICorrelatedSamplingData* data)
{
	CSDataBulkSplines* d = dynamic_cast<CSDataBulkSplines*>(data);
	d->splineSumsD = this->splineSumsD;
	d->splineSumsD2 = this->splineSumsD2;
	d->otherLocalOperators = this->otherLocalOperators;
}

}
