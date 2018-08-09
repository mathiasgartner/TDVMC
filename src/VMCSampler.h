#include "Constants.h"
#include "MathOperators.h"
#include "Utils.h"

#include <random>
#include <iostream>
#include <vector>

using namespace std;

namespace VMCSampler
{

mt19937_64 generator;
uniform_real_distribution<double> distUniform(0.0, 1.0);
normal_distribution<double> distNormal(0.0, 1.0);

vector<vector<double> > R = { { -0.3747973297263484937502653, 0.791188529547497410021606, -0.496517804417985431086891 }, { -1.1736960854695439593342599, 1.859406095754607690651028, -0.167749906139277982219937 }, { 1.106703368283721289344612, 0.3967881826152037660904170, 0.745481393160986272050650 }, { 0.166280993725784753678454, -0.525535338356522885305822, 1.226200881490768779258360 }, { -0.494024199375921568844206, -0.4336484606613097980698512, 1.4227506924490960216189706 }, { -1.029980674906454396477784, 0.374201238817484238552424, 1.8964700652838644145958824 }, { -1.2691909164086965233764204, 1.135905620016723460707908, 1.705911972428218348341034 }, { -0.837612353526964170669089, 0.292782495033677037099551, -0.4363637829083870656177169 }, { -1.9943686703070682142424630,
		-1.7622749308668943513112026, 0.1688419537399852288217517 }, { 0.4337663671475984195069486, -1.060011644702619548752409, 0.401150425422599710145732 }, { 1.835804101442818137002178, 0.909779199025479101692326, -1.680485893626638471687329 }, { 1.249637065513354627910303, -0.009685903615082924744684, -1.4011311342640198063236312 }, { 0.257482679181620710551215, -1.262293694920391118330372, 1.657836531002989666205849 }, { -0.3658137851602987211663276, -1.509938300440666125723510, -1.4788658863267514220751764 }, { 0.6277965902617097526672296, -1.075991414840217430537450, -1.862579451522755391579267 }, { 0.0227299432198364570467675, -0.185001855248433599854252, -1.1343808391294096793444623 }, { 0.630220984860049071585308, -1.611802215031111984444578, -1.374826317254569829628963 }, {
		0.117276764623717610902531, -0.996124514356722556840396, -0.5328991724755240966260317 }, { -1.849376742438465726081631, 0.1651489449758898331310775, 0.7252528247888534451703890 }, { 1.0434109865133827099725750, 0.498241862624223319500061, 1.944698042540338178696402 }, { 1.668662934640515516093728, 1.304784951648059632134391, 1.080643290929643285380735 }, { -0.782306242764360781904998, -0.5238298673318837472834275, 1.029034505656639453263779 }, { 1.4317405186432665686879773, 0.4226501231122767876513535, -0.197986431871328250053921 }, { -1.6500334205943545384798199, -0.8594568328403937584880623, 1.111116897848219764455280 }, { -0.867269262181103783859726, 0.861766007682973622650025, 1.3348269377995478635057225 }, { -1.5217760847993702100211522, -1.0230556581571548235842783,
		-1.918576973267121843491623 }, { -0.0431219571382399635695037, 1.750461282577935762105881, 0.269118262923224271787603 }, { -1.8447528534884867212895188, 1.0418330400473649888226646, -1.012948752095269355777418 }, { -0.2015466442786646439344622, 0.019607818949985755807575, 1.6983465449436296879071051 }, { 0.368517675380473974655615, -1.3714253946951657781028189, 1.2718345689750840676879307 }, { -1.0788732521696715593861882, -0.448404771173095184622071, -0.438049444291740641688193 }, { 1.964427090113218810074613, 0.912098338866009328285145, 0.2607357790419158050099213 }, { -1.0145611057190357939816749, 0.066531843497365628081752, -1.366311784505683135648724 }, { -1.200440378279807873695972, -0.7762885655124875938781770, -1.934640099744321162233973 }, { 0.388193998653335370363493,
		-1.693972716952814039359510, 0.552664091551392289147771 }, { -0.614033618142592274580238, 0.2051216173120522512363095, -1.9951464249442927290090211 }, { 0.1632423453725948547798907, -0.952700643190929952197621, 1.379491604020614659020794 }, { 0.600766225511584650575969, 0.503301213567741712040515, 0.584191030522873688823893 }, { 0.261604121341250106524967, 1.6251877426778813529040235, -1.524464535794757935605048 }, { -1.5051856722352709994083853, 1.8793188367306328245831537, -1.907097238603341793350410 }, { 0.534866857668490069954714, -0.318143106918739704269683, -1.4889091764773638715269044 }, { 1.691985493892232739199244, 1.884266914823378158416745, 0.2192084352354193299561302 }, { -0.442775453335088720052681, -0.9613418054140581148203637, 0.577710866499472786017577 }, {
		1.953020405652668323170929, -1.2862809427783759019803256, -1.078105666066150547521829 }, { 1.484895128649426965239400, 0.9862828150193934106937377, 0.239886705607219141711539 }, { 0.274668241134470747510932, -1.037191806129143856196606, -1.264788216190652292425511 }, { -1.5297720138249388810436358, 1.538084759244215504736530, -0.747310417659420522795699 }, { 0.6458193113558650111372117, -1.145903212687628069943457, 0.134149829190292280145513 }, { 0.502378894167573974982588, -1.569585814969443049449183, -1.834607565933737305385876 }, { 0.539303796921039690914768, 0.158818616463165085406217, -1.847954585263579474485596 }, { 1.147096577742573231262213, 0.411455272871627641961823, 0.462548460609198741622095 }, { 0.1035628544349904700538900, -1.7091174959486465922964271,
		0.730342610840015282747118 }, { -0.913572396940097064543807, 1.597648541766616858694761, -1.3490048993150480782787781 }, { 0.023394049337923661369132, 1.752471687630148267089680, 0.1013586903825274276869095 }, { 1.337376964144112889698590, 1.036076617475572447801824, 0.7737809326012237143288530 }, { -0.359563201507469898388081, 0.492587618146945516173219, -0.6477987651172831817802944 }, { -1.390020302000003482589818, 1.829382248620277096051723, -1.8694248890302276322472608 }, { 1.304249182106303805994685, 1.7802915710022126916101115, 1.661228851762103886358091 }, { 0.596249516574644644606451, 1.608973899876048108126270, 1.9275166299870747366185242 }, { 1.3494750978586651513069228, -1.9705803791430582805332961, 1.1383313620632122820097720 }, { 0.401186936235301772057937,
		-0.4307958256549824227477075, 1.906664459491480556607712 }, { 0.4776085535199783294046938, 1.4374388237222177622243180, -1.4457732426999427843838930 }, { -1.7474387223026717208540504, 0.8664803943559462595658260, 0.864556501894327311674715 }, { 1.426047057865976075419212, 0.985475179006418011695700, 1.263935977777479902073309 } };
vector<double> params = { -0.3991563858936407749311570, -0.4025607852528659824642432, -0.3991404756233223882766481, -0.3891689680599620948520112, -0.3734284854571393585942474, -0.3530870147545706605463067, -0.3295219964121220423969305, -0.3041438715825346417176434, -0.2782380496090479105042448, -0.2528652859046025547229419, -0.2288093932582702660916141, -0.2065761406532180544104449, -0.1864239394360506507641873, -0.1684169152723559159667843, -0.1524795222474658318301266, -0.1384523156906905805296759, -0.1261320406049412923277231, -0.1153049838817266914414716, -0.1057659424977745732388001, -0.0973290783152249844301096, -0.0898326525164965528080785, -0.0831390140719473064168099, -0.0771328923939296057232795, -0.0717183115403551008082417, -0.0668153105289754356554965,
		-0.0623575519288611102020070, -0.0582891491250301860271854, -0.0545634175126436596348078, -0.0511404948483345292431501, -0.0479869123993664839500362, -0.0450735204125540731001642, -0.0423755377520902420940274, -0.0398711704410607062354366, -0.0375417801403124112846754, -0.0353708870185305696010047, -0.0333441055316978443578968, -0.0314485767544181171739837, -0.0296731076316355153743576, -0.0280076721885876532558424, -0.0264433466054896162966692, -0.0249721932738108595706272, -0.0235869446754888717476817, -0.0222811982854174937818126, -0.0210491657274511564934549, -0.0198855319560674072276729, -0.0187856768516828052939527, -0.0177451450125886700526223, -0.0167600109538467156866659, -0.0158267132219434836226668, -0.0149419346615850996695674, -0.0141026948113651250799050,
		-0.0133061597119528071675454, -0.0125498654097968755788406, -0.0118314776918466701971511, -0.0111487720937928774800341, -0.0104998228978675604838866, -0.0098827228398820320121043, -0.0092958312611040733175471, -0.0087375310098073786374551, -0.0082064041048637310221681, -0.0077009703807680756140752, -0.0072200955282515251465458, -0.0067625086736428701925106, -0.0063271587285942338177436, -0.0059129212826891106372096, -0.0055189246683271242499558, -0.0051442516725908257746447, -0.0047880547400074233155287, -0.0044495587059894658316095, -0.0041279996477454495434900, -0.0038226579442119349057372, -0.0035329550043144111910742, -0.0032582102656706275235765, -0.0029978846129339741140296, -0.0027514018247092493496542, -0.0025182737256249413358711, -0.0022980315230067605881958,
		-0.0020901746832201642360693, -0.0018943433501013430368676, -0.0017100814193137061670902, -0.0015370485105604574701393, -0.0013748659911501297063718, -0.0012231760470942315902065, -0.0010816740048847086567291, -0.0009500666199343951127260, -0.0008280421253218606071275, -0.0007153847399904308089480, -0.0006117976891144458152436, -0.0005170383558157485475240, -0.0004308825263693107673717, -0.0003531319549384136810381, -0.0002835860453521756876193, -0.0002220230445958768080925, -0.0001682713652937027864857, -0.0001222189006439725425003, -0.0000835596374585455752915, -0.0000524317433445170639670, -0.0000280346403921109622100, -0.0000116679550669113991739, 0.000000000000000000000000 };
int nAcceptances = 0;
int nTrials = 0;
double nodePointSpacing = 1.0;
double maxDistance = 2.0;

double energy1 = 0;
double energy2 = 0;
double energy2_1 = 0;
double energy2_2 = 0;

double random01()
{
	return distUniform(generator);
}

double randomNormal()
{
	return distNormal(generator);
}

double randomNormal(double sigma, double mu)
{
	return randomNormal() * sigma + mu;
}

int randomInt(int maxValue)
{
	//TODO: test for maxValue > 0 is only needed for Gaussian wavepacket simulation where only one  particle is used
	return maxValue > 0 ? ((int) floor(random01() * maxValue)) % maxValue : maxValue;
}

double GetWf()
{
	double sum = 0;
	vector<double> sumsPerK;
	sumsPerK.resize(N_PARAM);
	for (int k = 0; k < N_PARAM; k++)
	{
		double kSum = 0;
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < i; j++)
			{
				vector<double> vecrij(DIM);
				double rij, interval, bin, res, res2, res3;
				rij = VectorDisplacementNIC(R[i], R[j], vecrij);

				if (rij < maxDistance)
				{
					interval = rij / nodePointSpacing;
					bin = int(interval); //INFO: same as floor(interval) for positive values of "interval" but a little bit faster
					res = interval - bin;
					res2 = res * res;
					res3 = res2 * res;

					if (k == N_PARAM - 1)
					{
						if (bin == k - 1)
						{
							kSum += params[k] * (1.0 / 6.0 * (4.0 - 6.0 * res2 + 3.0 * res3));
							kSum += params[k] * (1.0 / 6.0 * (1.0 + 3.0 * res + 3.0 * res2 - 3.0 * res3));
							kSum += params[k] * (1.0 / 6.0 * pow(res, 3.0));
						}
						else if (bin == k - 2)
						{
							kSum += params[k] * (1.0 / 6.0 * (1.0 + 3.0 * res + 3.0 * res2 - 3.0 * res3));
							kSum += params[k] * (1.0 / 6.0 * pow(res, 3.0));
						}
						else if (bin == k - 3)
						{
							kSum += params[k] * (1.0 / 6.0 * pow(res, 3.0));
						}
					}
					else if (bin == k)
					{
						kSum += params[k] * (1.0 / 6.0 * pow(1.0 - res, 3.0));
					}
					else if (bin == k - 1)
					{
						kSum += params[k] * (1.0 / 6.0 * (4.0 - 6.0 * res2 + 3.0 * res3));
					}
					else if (bin == k - 2)
					{
						kSum += params[k] * (1.0 / 6.0 * (1.0 + 3.0 * res + 3.0 * res2 - 3.0 * res3));
					}
					else if (bin == k - 3)
					{
						kSum += params[k] * (1.0 / 6.0 * res3);
					}
				}
			}
		}
		sumsPerK[k] = kSum;
		sum += kSum;
	}
	for (unsigned int i = 0; i < sumsPerK.size(); i++)
	{
		//cout << sumsPerK[i]/params[i] << endl;
	}
	double wf = exp(sum);
	return wf;
}

void DoMetropolisStep()
{
	double p;
	bool sampleOkay = true;
	double wf = GetWf();
	int randomParticle = randomInt(N - 1);
	vector<double> oldPosition(R[randomParticle]);
	for (int i = 0; i < DIM; i++)
	{
		R[randomParticle][i] += randomNormal(0.5, 0.2);
	}
	double wfNew = GetWf();
	double wfQuotient = (wfNew * wfNew) / (wf * wf);

	if (!isfinite(wfQuotient) || !isfinite(wfNew) || !isfinite(wf))
	{
		sampleOkay = false;
		if (!isfinite(wfQuotient) && wfNew > 0 && wf == 0) //INFO: this can happen when a new timestep is startet and the wavefunction is zero for the first particle configuration
		{
			sampleOkay = true;
			wfQuotient = 1; //INFO: accept by 100%
		}
	}

	p = random01();
	if (!sampleOkay || wfQuotient < p)
	{
		for (int i = 0; i < DIM; i++)
		{
			R[randomParticle][i] = oldPosition[i];
		}
	}
	else
	{
		nAcceptances++;
	}
	nTrials++;
}

double GetEnergy()
{
	double energy = 0;
	energy1 = 0;
	energy2 = 0;
	energy2_1 = 0;
	energy2_2 = 0;

	for (int n = 0; n < N; n++)
	{
		vector<double> sum1 = { 0.0, 0.0, 0.0 };
		double sum2 = 0.0;
		double sum2_1 = 0.0;
		double sum2_2 = 0.0;
		for (int k = 0; k < N_PARAM; k++)
		{
			for (int i = 0; i < N; i++)
			{
				if (i != n)
				{
					vector<double> vecrni(DIM);
					double rni, interval, bin, res, res2;
					rni = VectorDisplacementNIC(R[n], R[i], vecrni);
					vector<double> erni_delta = (vecrni / rni) / nodePointSpacing;
					double _delta2 = 1.0 / (nodePointSpacing * nodePointSpacing);
					double two_delta_rni = 2.0 / (nodePointSpacing * rni);

					if (rni < maxDistance)
					{
						interval = rni / nodePointSpacing;
						bin = int(interval); //INFO: same as floor(interval) for positive values of "interval" but a little bit faster
						res = interval - bin;
						res2 = res * res;

						if (k == 81)
						{
							//cout << 81 << endl;
						}
						if (k == N_PARAM - 1)
						{
							double tmpD1;
							double tmpD2;
							if (bin == k - 1)
							{
								tmpD1 = 1.0 / 6.0 * (-12.0 * res + 9.0 * res2);
								tmpD2 = 1.0 / 6.0 * (-12.0 + 18.0 * res);
								sum1 += erni_delta * params[k] * tmpD1;
								sum2 += params[k] * (tmpD2 * _delta2 + tmpD1 * two_delta_rni);
								//sum2_1 += params[k] * (tmpD1 * two_delta_rni);
								//sum2_2 += params[k] * (tmpD2 * _delta2);

								tmpD1 = 1.0 / 6.0 * (3.0 + 6.0 * res - 9.0 * res2);
								tmpD2 = 1.0 / 6.0 * (6.0 - 18.0 * res);
								sum1 += erni_delta * params[k] * tmpD1;
								sum2 += params[k] * (tmpD2 * _delta2 + tmpD1 * two_delta_rni);
								//sum2_1 += params[k] * (tmpD1 * two_delta_rni);
								//sum2_2 += params[k] * (tmpD2 * _delta2);

								tmpD1 = 1.0 / 2.0 * pow(res, 2.0);
								tmpD2 = res;
								sum1 += erni_delta * params[k] * tmpD1;
								sum2 += params[k] * (tmpD2 * _delta2 + tmpD1 * two_delta_rni);
								//sum2_1 += params[k] * (tmpD1 * two_delta_rni);
								//sum2_2 += params[k] * (tmpD2 * _delta2);
							}
							else if (bin == k - 2)
							{
								tmpD1 = 1.0 / 6.0 * (3.0 + 6.0 * res - 9.0 * res2);
								tmpD2 = 1.0 / 6.0 * (6.0 - 18.0 * res);
								sum1 += erni_delta * params[k] * tmpD1;
								sum2 += params[k] * (tmpD2 * _delta2 + tmpD1 * two_delta_rni);
								//sum2_1 += params[k] * (tmpD1 * two_delta_rni);
								//sum2_2 += params[k] * (tmpD2 * _delta2);

								tmpD1 = 1.0 / 2.0 * pow(res, 2.0);
								tmpD2 = res;
								sum1 += erni_delta * params[k] * tmpD1;
								sum2 += params[k] * (tmpD2 * _delta2 + tmpD1 * two_delta_rni);
								//sum2_1 += params[k] * (tmpD1 * two_delta_rni);
								//sum2_2 += params[k] * (tmpD2 * _delta2);
							}
							else if (bin == k - 3)
							{
								tmpD1 = 1.0 / 2.0 * pow(res, 2.0);
								tmpD2 = res;
								sum1 += erni_delta * params[k] * tmpD1;
								sum2 += params[k] * (tmpD2 * _delta2 + tmpD1 * two_delta_rni);
								//sum2_1 += params[k] * (tmpD1 * two_delta_rni);
								//sum2_2 += params[k] * (tmpD2 * _delta2);
							}
						}
						else if (bin == k)
						{
							double tmpD1 = -1.0 / 2.0 * pow(1.0 - res, 2.0);
							double tmpD2 = 1.0 - res;
							sum1 += erni_delta * params[k] * tmpD1;
							sum2 += params[k] * (tmpD2 * _delta2 + tmpD1 * two_delta_rni);
							//sum2_1 += params[k] * (tmpD1 * two_delta_rni);
							//sum2_2 += params[k] * (tmpD2 * _delta2);
						}
						else if (bin == k - 1)
						{
							double tmpD1 = 1.0 / 6.0 * (-12.0 * res + 9.0 * res2);
							double tmpD2 = 1.0 / 6.0 * (-12.0 + 18.0 * res);
							sum1 += erni_delta * params[k] * tmpD1;
							sum2 += params[k] * (tmpD2 * _delta2 + tmpD1 * two_delta_rni);
							//sum2_1 += params[k] * (tmpD1 * two_delta_rni);
							//sum2_2 += params[k] * (tmpD2 * _delta2);
						}
						else if (bin == k - 2)
						{
							double tmpD1 = 1.0 / 6.0 * (3.0 + 6.0 * res - 9.0 * res2);
							double tmpD2 = 1.0 / 6.0 * (6.0 - 18.0 * res);
							sum1 += erni_delta * params[k] * tmpD1;
							sum2 += params[k] * (tmpD2 * _delta2 + tmpD1 * two_delta_rni);
							//sum2_1 += params[k] * (tmpD1 * two_delta_rni);
							//sum2_2 += params[k] * (tmpD2 * _delta2);
						}
						else if (bin == k - 3)
						{
							double tmpD1 = 1.0 / 2.0 * pow(res, 2.0);
							double tmpD2 = res;
							sum1 += erni_delta * params[k] * tmpD1;
							sum2 += params[k] * (tmpD2 * _delta2 + tmpD1 * two_delta_rni);
							//sum2_1 += params[k] * (tmpD1 * two_delta_rni);
							//sum2_2 += params[k] * (tmpD2 * _delta2);
						}
					}
				}
			}
		}
		double sum1abs = VectorDotProduct(sum1, sum1);
		energy += -(sum1abs + sum2);
		//cout << sum2 << endl;
		energy1 += sum1abs;
		energy2 += sum2;
		energy2_1 += sum2_1;
		energy2_2 += sum2_2;
	}

	return energy / (double) N;
}

}

