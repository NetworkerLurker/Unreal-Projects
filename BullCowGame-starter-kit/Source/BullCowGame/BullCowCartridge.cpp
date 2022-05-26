// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"

void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();
    
    const FString WordListPath = FPaths::ProjectContentDir() / TEXT("WordLists/HiddenWordList.txt");
    FFileHelper::LoadFileToStringArray(Words, *WordListPath);

    Isograms = GetValidWords(Words);

    SetupGame();
}

void UBullCowCartridge::OnInput(const FString& PlayerInput) // When the player hits enter
{
    if (bGameOver)
    {
        ClearScreen(); 
        SetupGame();
    }
    else //Checking PlayerGuess
    {
        ProcessGuess(PlayerInput); 
    }
}

void UBullCowCartridge::SetupGame()
{
    PrintLine(TEXT("Welcome to the ranch, partner!"));
    
    HiddenWord = Isograms[FMath::RandRange(0, Isograms.Num() -1)];
    Lives = HiddenWord.Len();
    bGameOver = false;

    PrintLine(TEXT("Please guess a %i letter isogram."), HiddenWord.Len());
    PrintLine(TEXT("You have %i lives remaining."), Lives);
    PrintLine(TEXT("Type your guess and press enter to \ncontinue."));
    PrintLine(TEXT("The HiddenWord is: '%s'."), *HiddenWord); //debug line
}

void UBullCowCartridge::EndGame()
{
    bGameOver = true;
    PrintLine(TEXT("\nPress enter to play again."));
}

void UBullCowCartridge::ProcessGuess(const FString& Guess)
{
    if (Guess == HiddenWord)
    {
        PrintLine(TEXT("YOU WIN!"));
        EndGame();
        return;
    }

    if (Guess.Len() != HiddenWord.Len())
    {
        PrintLine(TEXT("Incorrect Input. The word is %i characters long."), HiddenWord.Len());
        return;
    }

    if (!IsIsogram(Guess))
    {
        PrintLine(TEXT("No repeating letters allowed, guess again."));
        return;
    }

    //Removes Lives
    PrintLine(TEXT("Wrong! You have %i lives remaining.\nGuess again."), --Lives);

    if (Lives <= 0)
    {
        ClearScreen();
        PrintLine(TEXT("GAME OVER!"));
        PrintLine(TEXT("\nThe secret word was '%s'"), *HiddenWord);
        EndGame();
        return;
    }  

    //Show the player Bulls and cows
    FBullCowCount Score = GetBullCows(Guess);

    PrintLine(TEXT("You have %i Bulls and %i Cows"), Score.Bulls, Score.Cows);
}

bool UBullCowCartridge::IsIsogram(const FString& Word) const
{
    for (int32 Index = 0; Index < Word.Len(); Index++)
    {
        for(int32 Comparison = Index + 1; Comparison < Word.Len(); Comparison++)
        {
            if (Word[Index] == Word[Comparison])
            {
                return false;
            }
        }
    }
    return true;
}

TArray<FString> UBullCowCartridge::GetValidWords(const TArray<FString>& WordList) const
{
    TArray<FString> ValidWords;

    for (FString Word : WordList)
    {
        if (Word.Len() >= 4 && Word.Len() <= 8 && IsIsogram(Word))
        {
            ValidWords.Emplace(Word);
        }    
    }
    return ValidWords;
}

FBullCowCount UBullCowCartridge::GetBullCows(const FString& Guess) const
{
    FBullCowCount Count;

    for (int32 GuessIndex = 0; GuessIndex < Guess.Len(); GuessIndex++)
    {
        if (Guess[GuessIndex] == HiddenWord[GuessIndex])
        {
            Count.Bulls ++;
            continue;
        }

        for (int32 HiddenIndex = 0; HiddenIndex < HiddenWord.Len(); HiddenIndex++)
        {
            if (Guess[GuessIndex] == HiddenWord[HiddenIndex])
            {
                Count.Cows++;
                break;
            }
        } 
    }
    return Count;
}