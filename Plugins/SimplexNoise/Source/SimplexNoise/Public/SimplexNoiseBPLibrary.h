/*
SimplexNoise 1.0.0
-----
DevDad - Afan Olovcic @ www.art-and-code.com - 08/12/2015

This algorithm was originally designed by Ken Perlin, but my code has been
adapted and extended from the implementation written by Stefan Gustavson (stegu@itn.liu.se)
and modified to fit to Unreal Engine 4


* This is a clean, fast, modern and free Perlin Simplex noise function.
* If we change float to double it could be even faster but there is no double type in Blueprint
* All Public Functions are BlueprintCallable so they can be used in every blueprint


From DevDad and Dedicated to you and Unreal Community
Use it free for what ever you want
I only request that you mention me in the credits for your game in the way that feels most appropriate to you.

*/

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimplexNoiseBPLibrary.generated.h"


UCLASS()
class SIMPLEXNOISE_API USimplexNoiseBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
private:

	static unsigned char perm[];
	static float  grad(int hash, float x);
	static float  grad(int hash, float x, float y);
	static float  grad(int hash, float x, float y, float z);
	static float  grad(int hash, float x, float y, float z, float t);

public:

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static void setNoiseSeed(const int32& newSeed);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise1D(float x);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise2D(float x, float y);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise3D(float x, float y, float z);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise4D(float x, float y, float z, float w);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise2DFractal(float x, float y, int32 Octaves = 1, float Frequency = 1.0f, float Amplitude = 1.0f, float Lacunarity = 2.0f, float Persistence = 0.5f);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseFractal3D(float x, float y, float z, int32 Octaves = 1, float Frequency = 1.0f, float Amplitude = 1.0f, float Lacunarity = 2.0f, float Persistence = 0.5f);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseScaledFractal2D(float x, float y, float s = 1.0f, int32 Octaves = 1, float Frequency = 1.0f, float Amplitude = 1.0f, float Lacunarity = 2.0f, float Persistence = 0.5f);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseScaledFractal3D(float x, float y, float z, float s = 1.0f, int32 Octaves = 1, float Frequency = 1.0f, float Amplitude = 1.0f, float Lacunarity = 2.0f, float Persistence = 0.5f);


	// Scaled by float value
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseScaled1D(float x , float s);
	
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseScaled2D(float x, float y, float s);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseScaled3D(float x, float y, float z, float s);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseScaled4D(float x, float y, float z, float w, float s);

	// Return value in Range between two float numbers
	// Return Value is scaled by difference between rangeMin & rangeMax value

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseInRange1D(float x, float rangeMin, float rangeMax);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseInRange2D(float x, float y, float rangeMin, float rangeMax);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseInRange3D(float x, float y, float z, float rangeMin, float rangeMax);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseInRange4D(float x, float y, float z, float w, float rangeMin, float rangeMax);

};

