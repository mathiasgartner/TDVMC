#include "HardSphereBosons.h"

HardSphereBosons::HardSphereBosons(vector<double>& params, string configDirectory) :
		IPhysicalSystem(params, configDirectory)
{
	this->USE_NORMALIZATION_AND_PHASE = false;
	this->USE_NIC = true;
	this->USE_MOVE_COM_TO_ZERO = false;

	halfLength = 0;
	maxDistance = 0;

	numOfOtherLocalOperators = 0;
	grBinCount = 0;
	grBinStartIndex = 0;
	grMaxDistance = 0;
	grNodePointSpacing = 0;
	numOfkValues = 0;

	exponentNew = 0;
	changedParticleIndex = 0;

	Mode = 1;
	cutoff.resize(6);
	for (int i = 0; i < 6; i++)
	{
		cutoff[i] = params[i];
	}
	rc1 = 1;
	rc2 = 1;
}

void HardSphereBosons::InitSystem()
{
	halfLength = LBOX / 2.0;
	rc1 = 0.7 * halfLength;
	rc2 = halfLength;

	wf = 0.0;

	localEnergyR = 0;

	wfNew = 0.0;

	localEnergyR = 0;
	localEnergyI = 0;
	localOperators.resize(N_PARAM);
	ClearVector(localOperators);
	localOperatorsMatrix.resize(N_PARAM);
	for (auto &row : localOperatorsMatrix)
	{
		row.resize(N_PARAM);
	}
	ClearVector(localOperatorsMatrix);
	localOperatorlocalEnergyR.resize(N_PARAM);
	ClearVector(localOperatorlocalEnergyR);
	localOperatorlocalEnergyI.resize(N_PARAM);
	ClearVector(localOperatorlocalEnergyI);

	grBinCount = 100;
	grBinStartIndex = 3;
	grBins.resize(grBinCount);
	ClearVector(grBins);
	grMaxDistance = halfLength;
	grNodePointSpacing = grMaxDistance / (double) grBinCount;

	grBinVolumes.resize(grBinCount);
	ClearVector(grBinVolumes);
	for (int i = 0; i < grBinCount; i++)
	{
		grBinVolumes[i] = 4.0 * M_PI * pow(grNodePointSpacing * (i + 1), 3.0) / 3.0;
	}
	for (int i = grBinCount - 1; i > 0; i--)
	{
		grBinVolumes[i] = grBinVolumes[i] - grBinVolumes[i - 1];
	}


	numOfOtherExpectationValues = 3 + grBinCount;
	otherExpectationValues.resize(numOfOtherExpectationValues);
	ClearVector(otherExpectationValues);
}

void HardSphereBosons::CalculateOtherLocalOperators(vector<vector<double> >& R)
{
	//INFO: not needed
}

vector<double> HardSphereBosons::GetCenterOfMass(vector<vector<double> >& R)
{
	vector<double> com = { 0.0, 0.0, 0.0 };
	return com;
}

void HardSphereBosons::CalculateExpectationValues(vector<vector<double> >& R, vector<double>& uR, vector<double>& uI, double phiR, double phiI)
{
	double grBinInterval;
	int grBin;
	double kineticR = 0;
	double out = 0;
	vector<double> term1;
	term1.resize(3);
	ClearVector(term1);
	double term2 = 0;
	double term3 = 0;
	for (int k = 0; k < N; k++)
	{
		ClearVector(term1);
		term2 = 0;
		term3 = 0;
		for (int j = 0; j < N; j++)
		{
			vector<double> vecr(DIM);
			double rkj = VectorDisplacementNIC(R[k], R[j], vecr);
			if (j != k)
			{
				if (rkj < halfLength)
				{
					if (rkj < rc1)
					{
						double a_rkj = uR[0] / rkj;
						double one_a_rkj = 1 - a_rkj;
						double rkj3 = rkj * rkj * rkj;
						double rkj4 = rkj3 * rkj;
						vector<double> tmp1 = vecr * ((uR[0] / one_a_rkj) / rkj3);
						double tmp2 = ((uR[0] * uR[0]) / (one_a_rkj * one_a_rkj)) / rkj4;
						//cout << "term1=" << tmp1.dot(tmp1) << " term2=" << tmp2 << endl;
						term1 += tmp1;
						term2 += tmp2;
					}
					if (rkj >= rc1 && rkj <= rc2)
					{
						// Jastrow smooth(r,a);
						// VectorXd cutoff = smooth.smooth_cutoff(a,rc1,rc2);
						double f1 = 0;
						double f2 = 0;
						double f4 = 0;
						for (unsigned l = 0; l < 6; l++)
						{
							f1 += cutoff[5 - l] * pow(rkj, l);
						}
						for (unsigned l = 1; l < 6; l++)
						{
							f2 += cutoff[5 - l] * l * pow(rkj, l - 1);
							//							f3 += cutoff[5-l]*l*pow(rkj,l-2);
							//							f4 += l*(l+1)*cutoff[5-l]*pow(rkj,l-2);
						}
						for (unsigned l = 2; l < 6; l++)
						{
							f4 += l * (l - 1) * cutoff[5 - l] * pow(rkj, l - 2);
						}
						term1 += vecr * ((1 / f1) * (f2 / rkj));
						term2 += f2 * (pow(f1, -2)) * f2;
						term3 += (1 / f1) * (f4);
					}
				}
				//term3 += 3*((1/(pow(boson.distanceNIC(k,j).norm(),3))) - (1/(pow(boson.distanceNIC(k,j).norm(),2))))*(a/(1-(a/(boson.distanceNIC(k,j).norm()))));
			}

			if (k < j && rkj < grMaxDistance)
			{
				grBinInterval = rkj / grNodePointSpacing;
				grBin = int(grBinInterval);
				//grBins[grBin] += 1.0 / (double)pow(grBin + 1, 2);
				grBins[grBin] += 1.0 / grBinVolumes[grBin];
			}
		}
		out += -VectorNorm2(term1) + term2 - term3;
	}
	kineticR = out;
	localEnergyR = kineticR;

	otherExpectationValues[0] = kineticR;
	otherExpectationValues[1] = 0;
	otherExpectationValues[2] = wf;
	for (int i = 0; i < grBinCount; i++)
	{
		otherExpectationValues[i + grBinStartIndex] = grBins[i];
	}
}

void HardSphereBosons::CalculateExpectationValues(ICorrelatedSamplingData* sample, vector<double>& uR, vector<double>& uI, double phiR, double phiI)
{
	//INFO: not needed
}

void HardSphereBosons::CalculateAdditionalSystemProperties(vector<vector<double> >& R, vector<double>& uR, vector<double>& uI, double phiR, double phiI)
{
	//INFO: not needed
}

void HardSphereBosons::CalculateWavefunction(vector<vector<double> >& R, vector<double>& uR, vector<double>& uI, double phiR, double phiI)
{
	double jastrow = 0;
	double out = 1;
	vector<double> vecrni(DIM);
	for (int j = 0; j < N; j++)
	{
		for (int i = 0; i < j; i++)
		{
			if (Mode == 1)
			{
				double r = VectorDisplacementNIC(R[j], R[i], vecrni);

				if (r <= uR[0])
				{
					jastrow = 0;
				}
				else if (rc1 < r && r <= rc2)
				{
					jastrow = cutoff[0] * pow(r, 5) + cutoff[1] * pow(r, 4) + cutoff[2] * pow(r, 3) + cutoff[3] * pow(r, 2) + cutoff[4] * r + cutoff[5];
				}
				else if (r > uR[0] && r <= rc1)
				{
					jastrow = (1 - (uR[0] / r)) / (1 - uR[0] / rc2);
				}
				else
				{
					jastrow = 1;
				}

				out *= (1 + uR[0]) * jastrow;
			}
			else
			{
				//Jastrow factor(boson.distanceNIC(i, j), A, Alpha);
				//out *= factor.jastrow;
			}
		}
	}
	wf = out;
	wf = 1;
}

void HardSphereBosons::CalculateWavefunction(ICorrelatedSamplingData* sample, vector<double>& uR, vector<double>& uI, double phiR, double phiI)
{
	//INFO: not needed
}

void HardSphereBosons::CalculateWFChange(vector<vector<double> >& R, vector<double>& uR, vector<double>& uI, double phiR, double phiI, int changedParticleIndex, vector<double>& oldPosition)
{
	double out = 1;
	double oldValues = 1;
	double jastrow = 0;
	for (int i = 0; i < N; i++)
	{
		if (i != changedParticleIndex)
		{
			if (Mode == 1)
			{
//				out*= 1+A;
				vector<double> vecrni(DIM);
				double r = VectorDisplacementNIC(R[i], R[changedParticleIndex], vecrni);
				if (r <= halfLength)
				{
					if (r <= uR[0])
					{
						jastrow = 0;
					}
					else if (rc1 < r && r <= rc2)
					{
						jastrow = cutoff[0] * pow(r, 5) + cutoff[1] * pow(r, 4) + cutoff[2] * pow(r, 3) + cutoff[3] * pow(r, 2) + cutoff[4] * r + cutoff[5];
					}
					else if (r > uR[0] && r <= rc1)
					{
						jastrow = (1 - (uR[0] / r)) / (1 - uR[0] / rc2);
					}
					else
					{
						jastrow = 1;
					}
					out *= jastrow;
				}

				r = VectorDisplacementNIC(R[i], oldPosition, vecrni);
				if (r <= halfLength)
				{
					if (r <= uR[0])
					{
						jastrow = 0;
					}
					else if (rc1 < r && r <= rc2)
					{
						jastrow = cutoff[0] * pow(r, 5) + cutoff[1] * pow(r, 4) + cutoff[2] * pow(r, 3) + cutoff[3] * pow(r, 2) + cutoff[4] * r + cutoff[5];
					}
					else if (r > uR[0] && r <= rc1)
					{
						jastrow = (1 - (uR[0] / r)) / (1 - uR[0] / rc2);
					}
					else
					{
						jastrow = 1;
					}
					oldValues *= jastrow;
				}
			}
			else
			{
				//if ((boson.distanceNIC(i, n).norm()) <= ((boson.L) / 2))
				//{
				//	Jastrow factor(boson.distanceNIC(i, n), A, Alpha);
				//	out *= factor.jastrow;
				//}
			}
		}
	}
	wfNew = out / oldValues;
}

double HardSphereBosons::CalculateWFQuotient(vector<vector<double> >& R, vector<double>& uR, vector<double>& uI, double phiR, double phiI, int changedParticleIndex, vector<double>& oldPosition)
{
	CalculateWFChange(R, uR, uI, phiR, phiI, changedParticleIndex, oldPosition);
	//double wfQuotient = wfNew / wf;
	double wfQuotient = wfNew;

	return wfQuotient;
}

void HardSphereBosons::AcceptMove()
{
	wf = wfNew;
}

void HardSphereBosons::FillCorrelatedSamplingData(ICorrelatedSamplingData* data)
{
	//INFO: not needed
}
