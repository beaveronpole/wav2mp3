////
//// Created by beaver on 18.12.2019.
////
//
//#include "SignedIntWaveDataReader.h"
//
//using namespace std;
//
//void SignedIntWaveDataReader::fillDataStorage(vector<uint8_t> *rawBuf, uint32_t rawBufDataSize,
//                                              vector<vector<int32_t> *> *dataOut) {
//    // on start all variables are checked in parent's getData
//
//    //TODO try to read silence
//    //TODO check file size and data size
//    //TODO check if EOF
//    //TODO think about stream encoding
//
//    uint8_t shift_value = 31 - m_bitsPerSample;
//    int32_t value = 0;
//    uint32_t uvalue = 0;
//    for (int i = 0, ch = 0 ;
//         i < rawBufDataSize; //TODO think about last sample, it is after SIZE
//         i += m_bytesPerSample, ch = i%(m_channelsCount*m_bytesPerSample)==0?0:(ch+1)){
//        if (ch >= 2)
//            continue;
//
//        uvalue = (*((uint32_t*)(rawBuf->data()+i)) & m_mask)  ;
//        uvalue <<= shift_value;
//        value = *(int32_t*)(&uvalue);
////        dataOut->at(ch)->push_back( value << shift_value);
//        int32_t max_bits = pow(2, m_bitsPerSample-1);
//        dataOut->at(ch)->push_back( (int32_t)((double)value/(double)max_bits*(double)INT32_MAX) );
//    }
//}
