/*
 * This file is a part of the open source stm32plus library.
 * Copyright (c) 2011,2012,2013,2014 Andy Brown <www.andybrown.me.uk>
 * Please see website for licensing terms.
 */

#pragma once

// ensure the MCU series is correct
#ifndef STM32PLUS_F0
#error This class can only be used with the STM32F0 series
#endif


namespace stm32plus {

  /**
   * DMA feature to enable ADC data to be written to memory
   * @tparam TAdc The type of the ADC peripheral (Adc1<...>, Adc2<...> ... )
   *
   * The default configuration is to run in circular buffer mode with half-word
   * transfers. These values can be customised by accessing the DMA_InitTypeDef structure
   * after the constructor has completed.
   */

  template<class TAdc,bool TRequestAfterLastTransfer=true,uint32_t TPriority=DMA_Priority_High>
  class AdcDmaFeature : public DmaFeatureBase {

    public:
      AdcDmaFeature(Dma& dma);
      void beginRead(volatile void *dest,uint32_t count);
  };


  /**
   * Constructor, store the reference to the DMA base class
   * @param dma the base class reference
   */

  template<class TAdc,bool TRequestAfterLastTransfer,uint32_t TPriority>
  inline AdcDmaFeature<TAdc,TRequestAfterLastTransfer,TPriority>::AdcDmaFeature(Dma& dma)
    : DmaFeatureBase(dma) {

    ADC_TypeDef *adc;

    adc=(ADC_TypeDef *)TAdc::PERIPHERAL_BASE;

    _init.DMA_PeripheralBaseAddr=reinterpret_cast<uint32_t>(&(adc->DR));
    _init.DMA_DIR=DMA_DIR_PeripheralSRC;                      // 'peripheral' is source
    _init.DMA_PeripheralInc=DMA_PeripheralInc_Disable;        // 'peripheral' does not increment
    _init.DMA_MemoryInc=DMA_MemoryInc_Enable;                 // memory is incremented
    _init.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord; // transferring 16-bits
    _init.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord;         // transferring 16-bits
    _init.DMA_Mode=DMA_Mode_Circular;                         // is a circular buffer
    _init.DMA_Priority=TPriority;                             // user-configurable priority
    _init.DMA_M2M=DMA_M2M_Disable;                            // peripheral->memory configuration
  }


  /**
   * Start a transfer of data to the destination.
   *
   * @param[in] dest The destination of the transfer.
   * @param[in] count The number of bytes to transfer.
   */

  template<class TAdc,bool TRequestAfterLastTransfer,uint32_t TPriority>
  inline void AdcDmaFeature<TAdc,TRequestAfterLastTransfer,TPriority>::beginRead(volatile void *dest,uint32_t count) {

    DMA_Channel_TypeDef *peripheralAddress;

    // set up the parameters for this transfer

    _init.DMA_MemoryBaseAddr=reinterpret_cast<uint32_t>(dest);
    _init.DMA_BufferSize=count;

    // this class is always in a hierarchy with DmaPeripheral

    peripheralAddress=_dma;

    // disable and then re-enable

    DMA_Cmd(peripheralAddress,DISABLE);
    DMA_Init(peripheralAddress,&_init);
    DMA_Cmd(peripheralAddress,ENABLE);

    ADC_DMARequestModeConfig((ADC_TypeDef *)TAdc::PERIPHERAL_BASE,TRequestAfterLastTransfer ? ADC_DMAMode_Circular : ADC_DMAMode_OneShot);
    ADC_DMACmd((ADC_TypeDef *)TAdc::PERIPHERAL_BASE,ENABLE);
  }
}
