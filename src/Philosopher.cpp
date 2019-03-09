//
// Created by tobias on 3/9/19.
//

#include <chrono>
#include <thread>
#include <random>

#include "Philosopher.h"

Philosopher::Philosopher(const unsigned long id, const int maxThinkingTimeInMilliseconds,
                         const int maxEatingTimeInMilliseconds, const std::shared_ptr<Fork> &rightFork,
                         const std::shared_ptr<Fork> &leftFork,
                         const std::shared_ptr<std::atomic_bool> dinnerIsRunning,
                         const std::shared_ptr<RandomIntGenerator> randomIntGenerator,
                         const std::shared_ptr<Logger> logger) :
        id(id),
        maxThinkingTimeInMilliseconds(maxThinkingTimeInMilliseconds),
        maxEatingTimeInMilliseconds(maxEatingTimeInMilliseconds),
        rightFork(rightFork),
        leftFork(leftFork),
        logger(logger),
        randomIntGenerator(randomIntGenerator),
        dinnerInProgress(dinnerIsRunning) {}

void Philosopher::dine() {
    while (!dinnerInProgress->load()) {}
    logger->info("{} starts dining", id);
    while (dinnerInProgress->load()) {
        think();
        logger->info("{} takes left fork with id {}", id, leftFork->getId());
        takeFork(leftFork);
        logger->info("{} took left fork with id {}", id, leftFork->getId());
        logger->info("{} takes right fork with id {}", id, rightFork->getId());
        takeFork(rightFork);
        logger->info("{} took right fork with id {}", id, rightFork->getId());
        eat();
        putBackForks();
        logger->info("{} returned forks", id);
    }
    logger->info("{} stops dining", id);
}

void Philosopher::think() const {
    const auto thinkingTime = getRandomIntBetweenZeroAnd(maxThinkingTimeInMilliseconds);
    logger->info("{} starts thinking for {}ms", id, thinkingTime);
    std::this_thread::sleep_for(std::chrono::milliseconds(thinkingTime));
    logger->info("{} finished thinking", id);
}


void Philosopher::takeFork(std::shared_ptr<Fork> fork) {
    fork->use();
}

void Philosopher::eat() const {
    const auto eatingTime = getRandomIntBetweenZeroAnd(maxEatingTimeInMilliseconds);
    logger->info("{} starts eating for {}ms", id, eatingTime);
    std::this_thread::sleep_for(std::chrono::milliseconds(eatingTime));
    logger->info("{} finished eating", id);
}

void Philosopher::putBackForks() {
    const auto order = getRandomIntBetweenZeroAnd(1);
    if (order == 0) {
        rightFork->putBack();
        leftFork->putBack();
        logger->info("{} returned right and then left fork", id);
    } else {
        leftFork->putBack();
        rightFork->putBack();
        logger->info("{} returned left and then right fork", id);
    }
}

int Philosopher::getRandomIntBetweenZeroAnd(const int high) const {
    return randomIntGenerator->generateRandomIntBetween(0, high);
}

