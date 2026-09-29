% ULSIM command line
% ./nr_ulsim -m 27 -u 1 -R 51 -r 51 -s 300 -L 4 -o

carrier = nrCarrierConfig;
carrier.NSizeGrid = 51;
carrier.NSlot = 8;
carrier.SubcarrierSpacing = 30;
carrier.CyclicPrefix = "normal";

pusch = nrPUSCHConfig;
pusch.PRBSet = 0:carrier.NSizeGrid-1;
pusch.SymbolAllocation = [0,12];
pusch.MappingType = "B";
pusch.NID = 0;
pusch.RNTI = 4660;
pusch.NumLayers = 1;
pusch.TransformPrecoding = false;
pusch.TransmissionScheme = "nonCodebook";
pusch.NumAntennaPorts = 1;
pusch.TPMI = 0;
pusch.Modulation = "64QAM";

pusch.DMRS.DMRSConfigurationType = 1;
pusch.DMRS.DMRSTypeAPosition = 2;
pusch.DMRS.NumCDMGroupsWithoutData = 1;
pusch.DMRS.DMRSAdditionalPosition = 0;
pusch.DMRS.NIDNSCID = 0;
pusch.DMRS.NRSID = 0;
pusch.DMRS.NSCID = 0;
pusch.DMRS.GroupHopping = 0;
pusch.DMRS.SequenceHopping = 0;

pusch.BetaOffsetACK = 20;
pusch.BetaOffsetCSI1 = 6.25;
pusch.BetaOffsetCSI2 = 1;
pusch.UCIScaling = 1;

[puschIndices,puschIndicesInfo] = nrPUSCHIndices(carrier,pusch);

dmrsLayerSymbols = nrPUSCHDMRS(carrier,pusch);
dmrsLayerIndices = nrPUSCHDMRSIndices(carrier,pusch);

run("../../../cmake_targets/ran_build/build/txsig0.m");
rxGrid = nrOFDMDemodulate(carrier,txs0);
[estChannelGrid,noiseEst] = nrChannelEstimate(carrier,rxGrid,dmrsLayerIndices,dmrsLayerSymbols);
[puschRx,puschHest] = nrExtractResources(puschIndices,rxGrid,estChannelGrid);
[puschEq,csi] = nrEqualizeMMSE(puschRx,puschHest,noiseEst);

decodeULSCH = nrULSCHDecoder;
decodeULSCH.MultipleHARQProcesses = false;
decodeULSCH.CBGTransmission = false;
decodeULSCH.TargetCodeRate = 910 / 1024;
decodeULSCH.LDPCDecodingAlgorithm = 'Normalized min-sum';
decodeULSCH.MaximumLDPCIterationCount = 6;
decodeULSCH.TransportBlockLength = 37896;

oack = 3;
ocsi1 = 0;
ocsi2 = 0;
[ulschLLRs,rxSymbols] = nrPUSCHDecode(carrier,pusch,decodeULSCH.TargetCodeRate,...
    decodeULSCH.TransportBlockLength,oack,ocsi1,ocsi2,puschEq,noiseEst);
rmInfo = nrULSCHInfo(pusch,decodeULSCH.TargetCodeRate,...
    decodeULSCH.TransportBlockLength,oack,ocsi1,ocsi2);
[culsch,cack,ccsi1,ccsi2] = nrULSCHDemultiplex(pusch,decodeULSCH.TargetCodeRate,...
    decodeULSCH.TransportBlockLength,oack,ocsi1,ocsi2,ulschLLRs);

plot(rxSymbols,'*');

decodeULSCH.reset();
rv = 0; % Redundancy version for decoding
[decodedBits,blkerr] = decodeULSCH(culsch,pusch.Modulation,pusch.NumLayers,rv);
% Display the decoded bits
disp('Block error:');
disp(blkerr);

ucibits = nrUCIDecode(cack,oack);
% Display the decoded UCI bits
disp('Decoded UCI bits:');
disp(ucibits);
